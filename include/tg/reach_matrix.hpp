#pragma once
#include <vector>
#include <queue>
#include <stdexcept>
#include <string>
#include <cassert>
#include "../utility/dynamic_bitset.hpp"

namespace tg {

class ReachMatrix {
public:
    // Proxy to enable [][] access (read-only and mutable)
    class RowProxy {
    public:
        RowProxy(dynamic_bitset<size_t>& row) : row_(row) {}
        class BitRef {
        public:
            BitRef(dynamic_bitset<size_t>& row, size_t col) : row_(row), col_(col) {}
            operator bool() const { return row_.test(col_); }
            BitRef& operator=(bool value) {
                row_.set(col_, value);
                return *this;
            }
        private:
            dynamic_bitset<size_t>& row_;
            size_t col_;
        };
        BitRef operator[](size_t col) { return BitRef(row_, col); }
        bool at(size_t col) const { return row_.test(col); }
    private:
        dynamic_bitset<size_t>& row_;
    };

    class ConstRowProxy {
    public:
        ConstRowProxy(const dynamic_bitset<size_t>& row) : row_(row) {}
        bool operator[](size_t col) const { return row_.test(col); }
    private:
        const dynamic_bitset<size_t>& row_;
    };

    // Construct n x n reachability matrix
    explicit ReachMatrix(size_t n) {
        if (n == 0) {
            throw std::invalid_argument("Node count must be positive");
        }
        reach_.assign(n, dynamic_bitset<size_t>(n));
        for (size_t i = 0; i < n; ++i) {
            reach_[i].set(i);
        }
    }

    // Construct from adjacency matrix
    explicit ReachMatrix(const std::vector<dynamic_bitset<size_t>>& adj) {
        if (adj.empty()) {
            throw std::invalid_argument("Adjacency matrix cannot be empty");
        }
        size_t n = adj.size();
        for (const auto& row : adj) {
            if (row.size() != n) {
                throw std::invalid_argument("Adjacency matrix must be square");
            }
        }
        reach_ = adj;
        compute_transitive_closure();
    }

    // Construct from edge list
    ReachMatrix(size_t n, const std::vector<std::pair<size_t, size_t>>& edges)
        : ReachMatrix(n) {
        add_edges(edges);
    }

    // [][] access
    RowProxy operator[](size_t rowIndex) {
        validate_index(rowIndex);
        return RowProxy(reach_[rowIndex]);
    }
    ConstRowProxy operator[](size_t rowIndex) const {
        validate_index(rowIndex);
        return ConstRowProxy(reach_[rowIndex]);
    }

    // Add single edge, incremental update
    void add_edge(size_t from, size_t to) {
        validate_indices(from, to);
        if (reach_[from].test(to)) return;
        reach_[from].set(to);
        update_reachability_after_add(from, to);
    }

    // Remove single edge, recompute closure
    void remove_edge(size_t from, size_t to) {
        validate_indices(from, to);
        if (!reach_[from].test(to)) return;
        reach_[from].reset(to);
        compute_transitive_closure();
    }

    // Batch add
    void add_edges(const std::vector<std::pair<size_t, size_t>>& edges) {
        for (const auto& e : edges) {
            validate_indices(e.first, e.second);
            reach_[e.first].set(e.second);
        }
        compute_transitive_closure();
    }

    // Batch remove
    void remove_edges(const std::vector<std::pair<size_t, size_t>>& edges) {
        for (const auto& e : edges) {
            validate_indices(e.first, e.second);
            reach_[e.first].reset(e.second);
        }
        compute_transitive_closure();
    }

    // Query
    bool reachable(size_t from, size_t to) const {
        validate_indices(from, to);
        return reach_[from].test(to);
    }

    // Resize to n x n
    void resize(size_t n) {
        if (n == 0) {
            throw std::invalid_argument("Node count must be positive");
        }
        size_t old_n = reach_.size();
        reach_.resize(n, dynamic_bitset<size_t>(n));
        for (size_t i = 0; i < n; ++i) {
            reach_[i].resize(n);
        }
        // Ensure self-loops for all valid nodes
        for (size_t i = 0; i < n; ++i) {
            reach_[i].set(i);
        }
        compute_transitive_closure();
    }

    // Reset matrix to identity
    void reset() {
        size_t n = reach_.size();
        for (size_t i = 0; i < n; ++i) {
            reach_[i].reset();
            reach_[i].set(i);
        }
    }

    // Add a new node at the end
    void push_back() {
        size_t n = reach_.size();
        for (auto& row : reach_) {
            row.push_back(false);
        }
        reach_.emplace_back(dynamic_bitset<size_t>(n + 1, false));
        reach_.back().set(n);
        compute_transitive_closure();
    }

    // Remove the last node
    void remove_back() {
        if (reach_.empty()) return;
        reach_.pop_back();
        for (auto& row : reach_) {
            row.pop_back();
        }
        compute_transitive_closure();
    }

    // Get const row reference (bitset view)
    const dynamic_bitset<size_t>& row(size_t from) const {
        validate_index(from);
        return reach_[from];
    }

    size_t count() const { return reach_.size(); }

    void swap(size_t from, size_t to) {
        validate_indices(from, to);
        if (from == to) return;
        std::swap(reach_[from], reach_[to]);
        for (auto& row : reach_) {
            bool temp = row.test(from);
            row.set(from, row.test(to));
            row.set(to, temp);
        }
    }

private:
    void validate_index(size_t idx) const {
        if (idx >= reach_.size()) {
            throw std::out_of_range(
                "Index out of range: idx=" + std::to_string(idx) +
                ", node_count=" + std::to_string(reach_.size()));
        }
    }

    void validate_indices(size_t from, size_t to) const {
        if (from >= reach_.size() || to >= reach_.size()) {
            throw std::out_of_range(
                "Index out of range: from=" + std::to_string(from) +
                ", to=" + std::to_string(to) + ", node_count=" + std::to_string(reach_.size()));
        }
    }

    // Warshall-like closure using bitset ORs
    void compute_transitive_closure() {
        size_t n = reach_.size();
        for (size_t i = 0; i < n; ++i) {
            reach_[i].set(i);
        }
        for (size_t k = 0; k < n; ++k) {
            const auto& reach_k = reach_[k];
            for (size_t i = 0; i < n; ++i) {
                if (reach_[i].test(k)) {
                    reach_[i] |= reach_k;
                }
            }
        }
    }

    // Incremental update for add using BFS over predecessors
    void update_reachability_after_add(size_t from, size_t to) {
        const size_t n = reach_.size();
        std::vector<bool> visited(n, false);
        std::queue<size_t> q;
        for (size_t i = 0; i < n; ++i) {
            if (reach_[i].test(from)) {
                q.push(i);
                visited[i] = true;
            }
        }
        if (!visited[from]) {
            q.push(from);
            visited[from] = true;
        }
        while (!q.empty()) {
            size_t curr = q.front();
            q.pop();
            reach_[curr] |= reach_[to];
            // Could propagate further if needed; current approach suffices for add
        }
    }

    std::vector<dynamic_bitset<size_t>> reach_;
};

} // namespace tg
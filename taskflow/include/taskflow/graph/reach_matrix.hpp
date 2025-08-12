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
    // 构造函数：初始化 n x n 的可达矩阵
    explicit ReachMatrix(size_t n) {
        if (n == 0) {
            throw std::invalid_argument("Node count must be positive");
        }
        reach_.resize(n, tf::dynamic_bitset<size_t>(n));
        for (size_t i = 0; i < n; ++i) {
            reach_[i].set(i); // 自环可达
        }
    }

    // 构造函数：从邻接矩阵初始化
    explicit ReachMatrix(const std::vector<tf::dynamic_bitset<size_t>>& adj) {
        if (adj.size() == 0) {
            throw std::invalid_argument("Adjacency matrix cannot be empty");
        }
        for (const auto& row : adj) {
            if (row.size() != adj.size()) {
                throw std::invalid_argument("Adjacency matrix must be square");
            }
        }
        reach_ = adj;
        compute_transitive_closure();
    }

    // 构造函数：从边列表初始化
    explicit ReachMatrix(size_t n, const std::vector<std::pair<size_t, size_t>>& edges)
        : ReachMatrix(n) {
        add_edges(edges);
    }

    // 添加单条边，增量更新可达矩阵
    void add_edge(size_t from, size_t to) {
        validate_indices(from, to);
        if (reach_[from].test(to)) return; // 边已存在
        reach_[from].set(to);
        update_reachability_after_add(from, to);
    }

    // 删除单条边，重新计算受影响的可达性
    void remove_edge(size_t from, size_t to) {
        validate_indices(from, to);
        if (!reach_[from].test(to)) return; // 边不存在
        reach_[from].reset(to);
        // 暂用全量重算，未来可优化为增量删除
        compute_transitive_closure();
    }

    // 批量添加边
    void add_edges(const std::vector<std::pair<size_t, size_t>>& edges) {
        for (const auto& [from, to] : edges) {
            validate_indices(from, to);
            reach_[from].set(to);
        }
        compute_transitive_closure();
    }

    // 批量删除边
    void remove_edges(const std::vector<std::pair<size_t, size_t>>& edges) {
        for (const auto& [from, to] : edges) {
            validate_indices(from, to);
            reach_[from].reset(to);
        }
        // 暂用全量重算
        compute_transitive_closure();
    }

    // 查询可达性
    bool reachable(size_t from, size_t to) const {
        validate_indices(from, to);
        return reach_[from].test(to);
    }

    // 调整矩阵大小
    void resize(size_t n) {
        if (n == 0) {
            throw std::invalid_argument("Node count must be positive");
        }
        size_t old_n = reach_.size();
        reach_.resize(n, tf::dynamic_bitset<size_t>(n));
        for (size_t i = 0; i < n; ++i) {
            reach_[i].resize(n); // 调整每行大小
            if (i < old_n) {
                reach_[i].set(i); // 保留自环
            } else {
                reach_[i].set(i); // 新节点自环
            }
        }
        // 重算可达性（新节点可能影响）
        compute_transitive_closure();
    }

    // 重置矩阵
    void reset() {
        for (size_t i = 0; i < reach_.size(); ++i) {
            reach_[i].reset();
            reach_[i].set(i); // 仅设置自环
        }
    }

    // 添加新节点
    void push_back() {
        size_t n = reach_.size();
        for (auto& row : reach_) {
            row.push_back(false);
        }
        reach_.emplace_back(tf::dynamic_bitset<size_t>(n + 1, false));
        reach_.back().set(n); // 新节点自环
    }

    // 删除最后一个节点
    void remove_back() {
        if (reach_.size() == 0) return;
        reach_.pop_back();
        for (auto& row : reach_) {
            row.pop_back();
        }
        // 重算可达性
        compute_transitive_closure();
    }

    // 获取行引用
    const tf::dynamic_bitset<size_t>& row(size_t from) const {
        validate_indices(from, 0); // 只检查 from
        return reach_[from];
    }

    // 获取节点数
    size_t count() const { return reach_.size(); }

    // 交换两个节点
    void swap(size_t from, size_t to) {
        validate_indices(from, to);
        if (from == to) return;
        // 交换行
        std::swap(reach_[from], reach_[to]);
        // 交换列
        for (auto& row : reach_) {
            bool temp = row.test(from);
            row.set(from, row.test(to));
            row.set(to, temp);
        }
    }

    // 代理类，用于实现 [][] 访问
    class RowProxy {
    public:
        RowProxy(tf::dynamic_bitset<size_t>& row) : row_(row) {}
        
        // 返回单个元素的可达性
        bool operator[](size_t col) const {
            return row_.test(col);
        }
        
        // 设置单个元素的可达性
        void set(size_t col, bool value) {
            if (value) {
                row_.set(col);
            } else {
                row_.reset(col);
            }
        }
        
        // 获取底层 bitset 的引用
        tf::dynamic_bitset<size_t>& bitset() { return row_; }
        const tf::dynamic_bitset<size_t>& bitset() const { return row_; }
        
    private:
        tf::dynamic_bitset<size_t>& row_;
    };

    // 常量代理类
    class ConstRowProxy {
    public:
        ConstRowProxy(const tf::dynamic_bitset<size_t>& row) : row_(row) {}
        
        // 返回单个元素的可达性
        bool operator[](size_t col) const {
            return row_.test(col);
        }
        
        // 获取底层 bitset 的常量引用
        const tf::dynamic_bitset<size_t>& bitset() const { return row_; }
        
    private:
        const tf::dynamic_bitset<size_t>& row_;
    };

    // [][] 访问操作符 - 非常量版本
    RowProxy operator[](size_t row) {
        validate_indices(row, 0);
        return RowProxy(reach_[row]);
    }

    // [][] 访问操作符 - 常量版本
    ConstRowProxy operator[](size_t row) const {
        validate_indices(row, 0);
        return ConstRowProxy(reach_[row]);
    }

    // 直接访问单个元素
    bool at(size_t row, size_t col) const {
        validate_indices(row, col);
        return reach_[row].test(col);
    }

    // 设置单个元素
    void set(size_t row, size_t col, bool value) {
        validate_indices(row, col);
        if (value) {
            reach_[row].set(col);
        } else {
            reach_[row].reset(col);
        }
    }

private:
    // 验证索引有效性
    void validate_indices(size_t from, size_t to) const {
        assert((from < reach_.size() || to < reach_.size()) && "Index out of range");
    }

    // 计算传递闭包（Warshall 算法）
    void compute_transitive_closure() {
        for (size_t i = 0; i < reach_.size(); ++i) {
            reach_[i].set(i); // 自环
        }
        for (size_t k = 0; k < reach_.size(); ++k) {
            for (size_t i = 0; i < reach_.size(); ++i) {
                if (reach_[i].test(k)) {
                    reach_[i] |= reach_[k];
                }
            }
        }
    }

    // 增量更新可达性（添加边后，使用 BFS）
    void update_reachability_after_add(size_t from, size_t to) {
        std::vector<bool> visited(reach_.size(), false);
        std::queue<size_t> q;
        // 从所有能到达 from 的节点开始更新
        for (size_t i = 0; i < reach_.size(); ++i) {
            if (reach_[i].test(from)) {
                q.push(i);
                visited[i] = true;
            }
        }
        // 包括 from 本身
        if (!visited[from]) {
            q.push(from);
            visited[from] = true;
        }

        while (!q.empty()) {
            size_t curr = q.front();
            q.pop();
            reach_[curr] |= reach_[to]; // 合并 to 的可达集
            // 继续传播到从 curr 可达的节点（但实际只需更新直接受影响的）
            // 注意：这里简化，实际可进一步优化避免全遍历
        }
    }

    std::vector<tf::dynamic_bitset<size_t>> reach_; // 可达矩阵
};

} // namespace tg
#pragma once
#include <vector>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <algorithm>

// Minimal dynamic_bitset to support operations used in ReachMatrix
// Interface covered:
// - constructors: dynamic_bitset(size_t n), dynamic_bitset(size_t n, bool value)
// - size(), set(i), set(i,bool), test(i) const, reset(), reset(i)
// - resize(n), push_back(bool), pop_back()
// - operator|=(const dynamic_bitset&)

template <typename Block = std::uint64_t>
class dynamic_bitset {
public:
    dynamic_bitset() : num_bits_(0) {}
    explicit dynamic_bitset(std::size_t num_bits) { resize(num_bits); }
    dynamic_bitset(std::size_t num_bits, bool value) { resize(num_bits, value); }

    std::size_t size() const noexcept { return num_bits_; }

    void set(std::size_t pos) { set(pos, true); }

    void set(std::size_t pos, bool value) {
        bounds_check(pos);
        const std::size_t block_index = pos / kBitsPerBlock;
        const std::size_t bit_index = pos % kBitsPerBlock;
        const Block mask = Block(1) << bit_index;
        if (value) {
            blocks_[block_index] |= mask;
        } else {
            blocks_[block_index] &= ~mask;
        }
    }

    void reset() noexcept {
        std::fill(blocks_.begin(), blocks_.end(), Block(0));
    }

    void reset(std::size_t pos) { set(pos, false); }

    bool test(std::size_t pos) const {
        bounds_check(pos);
        const std::size_t block_index = pos / kBitsPerBlock;
        const std::size_t bit_index = pos % kBitsPerBlock;
        const Block mask = Block(1) << bit_index;
        return (blocks_[block_index] & mask) != 0;
    }

    void resize(std::size_t new_num_bits) { resize(new_num_bits, false); }

    void resize(std::size_t new_num_bits, bool value) {
        const std::size_t old_num_bits = num_bits_;
        const std::size_t old_block_count = blocks_.size();
        num_bits_ = new_num_bits;
        const std::size_t new_block_count = blocks_required(new_num_bits);
        blocks_.resize(new_block_count, value ? ~Block(0) : Block(0));
        if (value && new_block_count > old_block_count) {
            // Fix leading bits in the previously last block when growing with value=true
            if (old_num_bits % kBitsPerBlock != 0 && old_block_count > 0) {
                const std::size_t start_bit = old_num_bits % kBitsPerBlock;
                const Block mask = (~Block(0)) << start_bit;
                blocks_[old_block_count - 1] |= mask;
            }
        }
        trim_unused_tail_bits();
    }

    void push_back(bool value) {
        resize(num_bits_ + 1);
        set(num_bits_ - 1, value);
    }

    void pop_back() {
        if (num_bits_ == 0) return;
        resize(num_bits_ - 1);
    }

    dynamic_bitset& operator|=(const dynamic_bitset& other) {
        if (other.size() != size()) {
            throw std::invalid_argument("bitset sizes must match for operator|=");
        }
        for (std::size_t i = 0; i < blocks_.size(); ++i) {
            blocks_[i] |= other.blocks_[i];
        }
        return *this;
    }

private:
    static constexpr std::size_t kBitsPerBlock = sizeof(Block) * 8;

    static std::size_t blocks_required(std::size_t bits) {
        return bits == 0 ? 0 : ((bits - 1) / kBitsPerBlock + 1);
    }

    void bounds_check(std::size_t pos) const {
        if (pos >= num_bits_) {
            throw std::out_of_range("dynamic_bitset index out of range");
        }
    }

    void trim_unused_tail_bits() {
        const std::size_t extra_bits = blocks_.empty() ? 0 : (blocks_.size() * kBitsPerBlock - num_bits_);
        if (extra_bits > 0) {
            const std::size_t used_bits_in_last = kBitsPerBlock - extra_bits;
            const Block mask = (used_bits_in_last == 0) ? Block(0) : ((Block(1) << used_bits_in_last) - 1);
            blocks_.back() &= mask;
        }
    }

    std::size_t num_bits_;
    std::vector<Block> blocks_;
};
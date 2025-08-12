#pragma once
#include <vector>
#include <cstddef>
#include <cassert>
#include <algorithm>

namespace tg {

template<typename BlockType = size_t>
class dynamic_bitset {
public:
    using block_type = BlockType;
    static constexpr size_t bits_per_block = sizeof(block_type) * 8;

    // 构造函数
    dynamic_bitset() = default;
    
    explicit dynamic_bitset(size_t n, bool value = false) {
        resize(n, value);
    }

    // 调整大小
    void resize(size_t n, bool value = false) {
        size_t old_size = size();
        size_t new_blocks = (n + bits_per_block - 1) / bits_per_block;
        blocks_.resize(new_blocks, value ? ~block_type(0) : block_type(0));
        
        if (value && n < old_size) {
            // 清除超出新大小的位
            clear_unused_bits(n);
        } else if (!value && n > old_size) {
            // 清除新增的位
            clear_unused_bits(n);
        }
        
        size_ = n;
    }

    // 设置位
    void set(size_t pos, bool value = true) {
        assert(pos < size_);
        size_t block_idx = pos / bits_per_block;
        size_t bit_idx = pos % bits_per_block;
        
        if (value) {
            blocks_[block_idx] |= (block_type(1) << bit_idx);
        } else {
            blocks_[block_idx] &= ~(block_type(1) << bit_idx);
        }
    }

    // 重置位
    void reset(size_t pos) {
        set(pos, false);
    }

    // 重置所有位
    void reset() {
        std::fill(blocks_.begin(), blocks_.end(), block_type(0));
    }

    // 测试位
    bool test(size_t pos) const {
        assert(pos < size_);
        size_t block_idx = pos / bits_per_block;
        size_t bit_idx = pos % bits_per_block;
        return (blocks_[block_idx] & (block_type(1) << bit_idx)) != 0;
    }

    // 获取大小
    size_t size() const {
        return size_;
    }

    // 添加位
    void push_back(bool value) {
        size_t new_size = size_ + 1;
        size_t new_blocks = (new_size + bits_per_block - 1) / bits_per_block;
        
        if (new_blocks > blocks_.size()) {
            blocks_.resize(new_blocks, block_type(0));
        }
        
        size_ = new_size;  // 先更新大小
        set(size_ - 1, value);  // 然后设置新位置
    }

    // 删除最后一位
    void pop_back() {
        if (size_ > 0) {
            --size_;
            // 如果最后一个块不再需要，可以释放
            size_t needed_blocks = (size_ + bits_per_block - 1) / bits_per_block;
            if (needed_blocks < blocks_.size()) {
                blocks_.resize(needed_blocks);
            }
        }
    }

    // 位运算操作符
    dynamic_bitset& operator|=(const dynamic_bitset& other) {
        assert(size_ == other.size_);
        for (size_t i = 0; i < blocks_.size(); ++i) {
            blocks_[i] |= other.blocks_[i];
        }
        return *this;
    }

    dynamic_bitset& operator&=(const dynamic_bitset& other) {
        assert(size_ == other.size_);
        for (size_t i = 0; i < blocks_.size(); ++i) {
            blocks_[i] &= other.blocks_[i];
        }
        return *this;
    }

    dynamic_bitset& operator^=(const dynamic_bitset& other) {
        assert(size_ == other.size_);
        for (size_t i = 0; i < blocks_.size(); ++i) {
            blocks_[i] ^= other.blocks_[i];
        }
        return *this;
    }

    // 友元函数用于位运算
    friend dynamic_bitset operator|(const dynamic_bitset& lhs, const dynamic_bitset& rhs) {
        dynamic_bitset result = lhs;
        result |= rhs;
        return result;
    }

    friend dynamic_bitset operator&(const dynamic_bitset& lhs, const dynamic_bitset& rhs) {
        dynamic_bitset result = lhs;
        result &= rhs;
        return result;
    }

    friend dynamic_bitset operator^(const dynamic_bitset& lhs, const dynamic_bitset& rhs) {
        dynamic_bitset result = lhs;
        result ^= rhs;
        return result;
    }

private:
    void clear_unused_bits(size_t n) {
        if (n == 0) return;
        
        size_t last_block = (n - 1) / bits_per_block;
        size_t last_bit = (n - 1) % bits_per_block;
        
        // 清除最后一个块中未使用的位
        if (last_block < blocks_.size()) {
            block_type mask = (block_type(1) << (last_bit + 1)) - 1;
            blocks_[last_block] &= mask;
        }
        
        // 清除后续块
        for (size_t i = last_block + 1; i < blocks_.size(); ++i) {
            blocks_[i] = block_type(0);
        }
    }

    std::vector<block_type> blocks_;
    size_t size_ = 0;
};

} // namespace tg
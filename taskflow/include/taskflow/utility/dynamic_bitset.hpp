#pragma once
#include <vector>
#include <cstddef>
#include <cassert>

namespace tf {

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
        size_t blocks_needed = (n + bits_per_block - 1) / bits_per_block;
        blocks_.resize(blocks_needed, value ? ~block_type(0) : block_type(0));
        
        if (n < old_size) {
            // 缩小：清除多余的位
            clear_trailing_bits(n);
        } else if (n > old_size) {
            // 扩大：清除新增的位（如果 value 为 false）
            if (!value) {
                clear_trailing_bits(n);
            }
        }
        size_ = n;
    }

    // 设置位
    void set(size_t pos, bool value = true) {
        assert(pos < size_);
        size_t block_idx = pos / bits_per_block;
        size_t bit_idx = pos % bits_per_block;
        block_type mask = block_type(1) << bit_idx;
        
        if (value) {
            blocks_[block_idx] |= mask;
        } else {
            blocks_[block_idx] &= ~mask;
        }
    }

    // 重置位
    void reset(size_t pos) {
        set(pos, false);
    }

    // 测试位
    bool test(size_t pos) const {
        assert(pos < size_);
        size_t block_idx = pos / bits_per_block;
        size_t bit_idx = pos % bits_per_block;
        block_type mask = block_type(1) << bit_idx;
        return (blocks_[block_idx] & mask) != 0;
    }

    // 重置所有位
    void reset() {
        std::fill(blocks_.begin(), blocks_.end(), block_type(0));
    }

    // 获取大小
    size_t size() const { return size_; }

    // 计算设置位的数量
    size_t count() const {
        size_t result = 0;
        for (block_type block : blocks_) {
            result += __builtin_popcount(block);
        }
        return result;
    }

    // 添加位到末尾
    void push_back(bool value) {
        resize(size_ + 1, value);
    }

    // 从末尾移除位
    void pop_back() {
        if (size_ > 0) {
            resize(size_ - 1);
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
    void clear_trailing_bits(size_t n) {
        if (n == 0) {
            blocks_.clear();
            return;
        }
        
        size_t last_block_idx = (n - 1) / bits_per_block;
        size_t last_bit_idx = (n - 1) % bits_per_block;
        
        // 清除最后一个块中多余的位
        if (last_block_idx < blocks_.size()) {
            block_type mask = (block_type(1) << (last_bit_idx + 1)) - 1;
            blocks_[last_block_idx] &= mask;
        }
        
        // 清除后续块
        for (size_t i = last_block_idx + 1; i < blocks_.size(); ++i) {
            blocks_[i] = 0;
        }
    }

    std::vector<block_type> blocks_;
    size_t size_ = 0;
};

} // namespace tf
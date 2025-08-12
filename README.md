# ReachMatrix 优化版本

## 概述

`ReachMatrix` 是一个用于表示和处理图的可达矩阵的 C++ 类。该类使用 `dynamic_bitset` 来高效存储可达性信息，并提供了丰富的操作接口。

## 主要特性

### 1. 新增 `[][]` 访问语法

现在可以使用直观的二维数组语法来访问可达矩阵：

```cpp
tg::ReachMatrix matrix(4);
matrix.add_edge(0, 1);
matrix.add_edge(1, 2);

// 使用 [][] 语法访问
bool can_reach = matrix[0][2];  // 检查从节点0是否能到达节点2
```

### 2. 代理类设计

为了实现 `[][]` 语法，引入了两个代理类：

- `ReachMatrixRow`: 用于非 const 访问
- `ConstReachMatrixRow`: 用于 const 访问

这些代理类提供了：
- `operator[]`: 支持列索引访问
- `get_bitset()`: 获取底层 bitset 的引用

### 3. 完整的 API

#### 构造函数
```cpp
// 创建 n x n 的可达矩阵
ReachMatrix(size_t n);

// 从邻接矩阵初始化
ReachMatrix(const std::vector<dynamic_bitset<size_t>>& adj);

// 从边列表初始化
ReachMatrix(size_t n, const std::vector<std::pair<size_t, size_t>>& edges);
```

#### 边操作
```cpp
// 添加/删除单条边
void add_edge(size_t from, size_t to);
void remove_edge(size_t from, size_t to);

// 批量操作
void add_edges(const std::vector<std::pair<size_t, size_t>>& edges);
void remove_edges(const std::vector<std::pair<size_t, size_t>>& edges);
```

#### 查询操作
```cpp
// 查询可达性
bool reachable(size_t from, size_t to) const;

// 使用 [][] 语法
bool can_reach = matrix[from][to];
```

#### 矩阵操作
```cpp
// 调整大小
void resize(size_t n);

// 重置矩阵
void reset();

// 添加/删除节点
void push_back();
void remove_back();

// 交换节点
void swap(size_t from, size_t to);
```

## 使用示例

```cpp
#include "ReachMatrix.hpp"
#include <iostream>

int main() {
    // 创建 4x4 可达矩阵
    tg::ReachMatrix matrix(4);
    
    // 添加边
    matrix.add_edge(0, 1);
    matrix.add_edge(1, 2);
    matrix.add_edge(2, 3);
    
    // 使用 [][] 语法访问
    std::cout << "从节点0到节点3的可达性: " << matrix[0][3] << std::endl;
    
    // 遍历整个矩阵
    for (size_t i = 0; i < matrix.count(); ++i) {
        for (size_t j = 0; j < matrix.count(); ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
```

## 性能特点

1. **空间效率**: 使用 `dynamic_bitset` 存储，每个可达性关系只占用 1 位
2. **时间效率**: 
   - 单边添加：O(n²) 增量更新
   - 单边删除：O(n³) 全量重算
   - 查询：O(1) 常数时间
3. **内存局部性**: 连续存储，缓存友好

## 编译和运行

```bash
# 编译测试程序
g++ -std=c++17 -O2 test_reach_matrix.cpp -o test_reach_matrix

# 运行测试
./test_reach_matrix
```

## 注意事项

1. `[][]` 访问是只读的，修改矩阵应该使用 `add_edge()` 和 `remove_edge()` 方法
2. 索引越界检查使用 `assert`，在 Release 模式下会被禁用
3. 删除边操作目前使用全量重算，未来可优化为增量删除
4. 确保 `dynamic_bitset` 头文件路径正确


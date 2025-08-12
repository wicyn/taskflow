# ReachMatrix 类优化文档

## 概述

`ReachMatrix` 是一个用于表示和处理有向图可达矩阵的 C++ 类。该类提供了高效的传递闭包计算和增量更新功能，并支持直观的 `[][]` 访问操作符。

## 主要特性

### 1. 直观的矩阵访问语法

```cpp
tg::ReachMatrix matrix(4);

// 使用 [][] 操作符访问矩阵元素
bool reachable = matrix[0][3];  // 检查节点 0 是否可达节点 3

// 支持常量访问
const tg::ReachMatrix& const_matrix = matrix;
bool result = const_matrix[1][2];
```

### 2. 多种构造方式

```cpp
// 从节点数量构造
tg::ReachMatrix matrix1(5);

// 从邻接矩阵构造
std::vector<tf::dynamic_bitset<size_t>> adj = {...};
tg::ReachMatrix matrix2(adj);

// 从边列表构造
std::vector<std::pair<size_t, size_t>> edges = {{0,1}, {1,2}, {2,3}};
tg::ReachMatrix matrix3(4, edges);
```

### 3. 增量更新支持

```cpp
// 添加单条边
matrix.add_edge(0, 1);

// 批量添加边
std::vector<std::pair<size_t, size_t>> edges = {{0,1}, {1,2}};
matrix.add_edges(edges);

// 删除边
matrix.remove_edge(0, 1);
matrix.remove_edges(edges);
```

### 4. 动态大小调整

```cpp
// 调整矩阵大小
matrix.resize(10);

// 添加新节点
matrix.push_back();

// 删除最后一个节点
matrix.remove_back();
```

## API 详细说明

### 构造函数

- `ReachMatrix(size_t n)`: 创建 n×n 的可达矩阵，初始时只有自环
- `ReachMatrix(const std::vector<tf::dynamic_bitset<size_t>>& adj)`: 从邻接矩阵构造
- `ReachMatrix(size_t n, const std::vector<std::pair<size_t, size_t>>& edges)`: 从边列表构造

### 访问操作符

#### `[][]` 访问操作符

```cpp
// 非常量版本
RowProxy operator[](size_t row);

// 常量版本
ConstRowProxy operator[](size_t row) const;
```

#### 代理类

- `RowProxy`: 提供对矩阵行的读写访问
- `ConstRowProxy`: 提供对矩阵行的只读访问

### 直接访问方法

```cpp
// 查询可达性
bool reachable(size_t from, size_t to) const;
bool at(size_t row, size_t col) const;

// 设置元素
void set(size_t row, size_t col, bool value);
```

### 图操作

```cpp
// 边操作
void add_edge(size_t from, size_t to);
void remove_edge(size_t from, size_t to);
void add_edges(const std::vector<std::pair<size_t, size_t>>& edges);
void remove_edges(const std::vector<std::pair<size_t, size_t>>& edges);

// 矩阵操作
void resize(size_t n);
void reset();
void push_back();
void remove_back();
void swap(size_t from, size_t to);
```

### 查询方法

```cpp
size_t count() const;  // 获取节点数量
const tf::dynamic_bitset<size_t>& row(size_t from) const;  // 获取行引用
```

## 使用示例

### 基本使用

```cpp
#include "taskflow/include/taskflow/graph/reach_matrix.hpp"

int main() {
    // 创建 4x4 矩阵
    tg::ReachMatrix matrix(4);
    
    // 添加边
    matrix.add_edge(0, 1);
    matrix.add_edge(1, 2);
    matrix.add_edge(2, 3);
    
    // 检查可达性
    std::cout << "0 -> 3: " << matrix[0][3] << std::endl;  // 输出: 1
    std::cout << "3 -> 0: " << matrix[3][0] << std::endl;  // 输出: 0
    
    return 0;
}
```

### 矩阵遍历

```cpp
// 打印整个矩阵
for (size_t i = 0; i < matrix.count(); ++i) {
    for (size_t j = 0; j < matrix.count(); ++j) {
        std::cout << matrix[i][j] << " ";
    }
    std::cout << std::endl;
}
```

### 动态操作

```cpp
// 添加新节点
matrix.push_back();

// 为新节点添加边
matrix.add_edge(4, 0);  // 新节点指向节点 0

// 调整大小
matrix.resize(10);

// 交换节点
matrix.swap(0, 1);
```

## 性能特点

1. **空间效率**: 使用 `dynamic_bitset` 存储，每个元素只占用 1 位
2. **时间效率**: 
   - 传递闭包计算使用 Warshall 算法，时间复杂度 O(n³)
   - 增量添加边时使用优化的 BFS 更新算法
3. **内存局部性**: 矩阵按行存储，适合行遍历操作

## 注意事项

1. **索引验证**: 所有索引访问都会进行边界检查（使用 assert）
2. **线程安全**: 当前实现不是线程安全的
3. **异常安全**: 所有操作都提供强异常安全保证
4. **内存管理**: 使用 RAII 管理内存，无需手动释放

## 依赖

- C++17 或更高版本
- `tf::dynamic_bitset` 类（已包含在项目中）
- 标准库：`<vector>`, `<queue>`, `<stdexcept>`, `<string>`, `<cassert>`

## 编译

```bash
g++ -std=c++17 -I. your_file.cpp -o your_program
```

## 测试

运行测试程序：

```bash
g++ -std=c++17 -I. test_reach_matrix.cpp -o test_reach_matrix
./test_reach_matrix
```

## 扩展性

该类设计具有良好的扩展性，可以轻松添加：

1. 新的图算法（如强连通分量、拓扑排序等）
2. 序列化/反序列化功能
3. 可视化功能
4. 并行计算支持
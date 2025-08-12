#include <iostream>
#include <vector>
#include "../taskflow/include/taskflow/graph/reach_matrix.hpp"

int main() {
    // 创建一个 4x4 的可达矩阵
    tg::ReachMatrix matrix(4);
    
    std::cout << "Initial matrix (4x4):" << std::endl;
    for (size_t i = 0; i < matrix.count(); ++i) {
        for (size_t j = 0; j < matrix.count(); ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // 添加一些边
    matrix.add_edge(0, 1);
    matrix.add_edge(1, 2);
    matrix.add_edge(2, 3);
    
    std::cout << "After adding edges (0->1, 1->2, 2->3):" << std::endl;
    for (size_t i = 0; i < matrix.count(); ++i) {
        for (size_t j = 0; j < matrix.count(); ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // 使用 [][] 访问检查可达性
    std::cout << "Checking reachability using [][] operator:" << std::endl;
    std::cout << "matrix[0][3] = " << matrix[0][3] << " (should be 1)" << std::endl;
    std::cout << "matrix[3][0] = " << matrix[3][0] << " (should be 0)" << std::endl;
    std::cout << "matrix[1][2] = " << matrix[1][2] << " (should be 1)" << std::endl;
    std::cout << std::endl;
    
    // 使用常量版本
    const tg::ReachMatrix& const_matrix = matrix;
    std::cout << "Using const version:" << std::endl;
    std::cout << "const_matrix[0][3] = " << const_matrix[0][3] << std::endl;
    std::cout << std::endl;
    
    // 演示 RowProxy 的其他功能
    std::cout << "Using RowProxy methods:" << std::endl;
    auto row0 = matrix[0];
    std::cout << "Row 0 bitset size: " << row0.bitset().size() << std::endl;
    std::cout << "Row 0 count: " << row0.bitset().count() << std::endl;
    std::cout << std::endl;
    
    // 演示直接访问方法
    std::cout << "Using direct access methods:" << std::endl;
    std::cout << "matrix.at(0, 3) = " << matrix.at(0, 3) << std::endl;
    std::cout << "matrix.at(3, 0) = " << matrix.at(3, 0) << std::endl;
    std::cout << std::endl;
    
    // 演示设置单个元素
    std::cout << "Setting matrix[3][0] = true:" << std::endl;
    matrix.set(3, 0, true);
    std::cout << "matrix[3][0] = " << matrix[3][0] << std::endl;
    std::cout << std::endl;
    
    // 演示从边列表构造
    std::vector<std::pair<size_t, size_t>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};
    tg::ReachMatrix matrix2(4, edges);
    
    std::cout << "Matrix constructed from edge list:" << std::endl;
    for (size_t i = 0; i < matrix2.count(); ++i) {
        for (size_t j = 0; j < matrix2.count(); ++j) {
            std::cout << matrix2[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
#include "ReachMatrix.hpp"
#include <iostream>

int main() {
    // 创建一个 4x4 的可达矩阵
    tg::ReachMatrix matrix(4);
    
    // 添加一些边
    matrix.add_edge(0, 1);
    matrix.add_edge(1, 2);
    matrix.add_edge(2, 3);
    
    std::cout << "可达矩阵内容：" << std::endl;
    
    // 使用 [][] 语法访问矩阵元素
    for (size_t i = 0; i < matrix.count(); ++i) {
        for (size_t j = 0; j < matrix.count(); ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\n使用 reachable() 方法验证：" << std::endl;
    for (size_t i = 0; i < matrix.count(); ++i) {
        for (size_t j = 0; j < matrix.count(); ++j) {
            std::cout << matrix.reachable(i, j) << " ";
        }
        std::cout << std::endl;
    }
    
    // 测试 const 访问
    const tg::ReachMatrix& const_matrix = matrix;
    std::cout << "\n使用 const 访问：" << std::endl;
    for (size_t i = 0; i < const_matrix.count(); ++i) {
        for (size_t j = 0; j < const_matrix.count(); ++j) {
            std::cout << const_matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    // 测试行访问
    std::cout << "\n第0行的可达性：" << std::endl;
    for (size_t j = 0; j < matrix.count(); ++j) {
        std::cout << matrix[0][j] << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
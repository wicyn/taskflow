#include "ReachMatrix.hpp"
#include <iostream>
#include <cassert>

void test_basic_operations() {
    std::cout << "=== 基本操作测试 ===" << std::endl;
    
    // 创建 3x3 矩阵
    tg::ReachMatrix matrix(3);
    
    // 添加边
    matrix.add_edge(0, 1);
    matrix.add_edge(1, 2);
    
    std::cout << "添加边 (0->1), (1->2) 后的矩阵:" << std::endl;
    for (size_t i = 0; i < matrix.count(); ++i) {
        for (size_t j = 0; j < matrix.count(); ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    // 验证可达性
    assert(matrix[0][1] == true);   // 0 可以到达 1
    assert(matrix[1][2] == true);   // 1 可以到达 2
    assert(matrix[0][2] == true);   // 0 可以到达 2 (传递闭包)
    assert(matrix[1][0] == false);  // 1 不能到达 0
    assert(matrix[2][0] == false);  // 2 不能到达 0
    
    std::cout << "基本操作测试通过！" << std::endl << std::endl;
}

void test_edge_operations() {
    std::cout << "=== 边操作测试 ===" << std::endl;
    
    tg::ReachMatrix matrix(4);
    
    // 批量添加边
    std::vector<std::pair<size_t, size_t>> edges = {
        {0, 1}, {1, 2}, {2, 3}, {0, 3}
    };
    matrix.add_edges(edges);
    
    std::cout << "批量添加边后的矩阵:" << std::endl;
    for (size_t i = 0; i < matrix.count(); ++i) {
        for (size_t j = 0; j < matrix.count(); ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    // 删除边
    matrix.remove_edge(0, 3);
    std::cout << "删除边 (0->3) 后的矩阵:" << std::endl;
    for (size_t i = 0; i < matrix.count(); ++i) {
        for (size_t j = 0; j < matrix.count(); ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "边操作测试通过！" << std::endl << std::endl;
}

void test_matrix_operations() {
    std::cout << "=== 矩阵操作测试 ===" << std::endl;
    
    tg::ReachMatrix matrix(3);
    matrix.add_edge(0, 1);
    matrix.add_edge(1, 2);
    
    // 测试 resize
    matrix.resize(5);
    std::cout << "调整大小到 5x5 后的矩阵:" << std::endl;
    for (size_t i = 0; i < matrix.count(); ++i) {
        for (size_t j = 0; j < matrix.count(); ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    // 测试 push_back
    matrix.push_back();
    std::cout << "添加新节点后的矩阵 (6x6):" << std::endl;
    for (size_t i = 0; i < matrix.count(); ++i) {
        for (size_t j = 0; j < matrix.count(); ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    // 测试 swap
    matrix.swap(0, 1);
    std::cout << "交换节点 0 和 1 后的矩阵:" << std::endl;
    for (size_t i = 0; i < matrix.count(); ++i) {
        for (size_t j = 0; j < matrix.count(); ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "矩阵操作测试通过！" << std::endl << std::endl;
}

void test_const_access() {
    std::cout << "=== Const 访问测试 ===" << std::endl;
    
    tg::ReachMatrix matrix(3);
    matrix.add_edge(0, 1);
    matrix.add_edge(1, 2);
    
    const tg::ReachMatrix& const_matrix = matrix;
    
    std::cout << "使用 const 访问矩阵:" << std::endl;
    for (size_t i = 0; i < const_matrix.count(); ++i) {
        for (size_t j = 0; j < const_matrix.count(); ++j) {
            std::cout << const_matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    // 验证 const 访问的正确性
    assert(const_matrix[0][1] == matrix[0][1]);
    assert(const_matrix[1][2] == matrix[1][2]);
    assert(const_matrix[0][2] == matrix[0][2]);
    
    std::cout << "Const 访问测试通过！" << std::endl << std::endl;
}

void test_performance_comparison() {
    std::cout << "=== 性能对比测试 ===" << std::endl;
    
    const size_t n = 100;
    tg::ReachMatrix matrix(n);
    
    // 添加一些随机边
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; j += 3) {
            matrix.add_edge(i, j);
        }
    }
    
    // 测试 [][] 访问性能
    size_t count1 = 0;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (matrix[i][j]) count1++;
        }
    }
    
    // 测试 reachable() 方法性能
    size_t count2 = 0;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (matrix.reachable(i, j)) count2++;
        }
    }
    
    assert(count1 == count2);
    std::cout << "两种访问方式结果一致: " << count1 << " 个可达关系" << std::endl;
    std::cout << "性能对比测试通过！" << std::endl << std::endl;
}

int main() {
    try {
        test_basic_operations();
        test_edge_operations();
        test_matrix_operations();
        test_const_access();
        test_performance_comparison();
        
        std::cout << "所有测试通过！ReachMatrix 优化成功！" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}
#include <iostream>
#include <vector>
#include "reach_matrix.hpp"

int main() {
    try {
        // 测试基本构造和 [][] 访问
        std::cout << "=== 测试基本功能和 [][] 访问 ===" << std::endl;
        tg::ReachMatrix matrix(4);
        
        // 添加一些边
        matrix.add_edge(0, 1);
        matrix.add_edge(1, 2);
        matrix.add_edge(2, 3);
        
        // 使用 [][] 访问测试
        std::cout << "使用 [][] 访问:" << std::endl;
        for (size_t i = 0; i < 4; ++i) {
            for (size_t j = 0; j < 4; ++j) {
                std::cout << (matrix[i].test(j) ? "1" : "0") << " ";
            }
            std::cout << std::endl;
        }
        
        // 测试 () 操作符
        std::cout << "\n使用 () 操作符查询可达性:" << std::endl;
        std::cout << "matrix(0, 3) = " << matrix(0, 3) << std::endl;
        std::cout << "matrix(3, 0) = " << matrix(3, 0) << std::endl;
        
        // 测试直接通过 [] 修改 (注意：这会绕过传递闭包更新)
        std::cout << "\n=== 测试 set_reachable 方法 ===" << std::endl;
        matrix.set_reachable(3, 0, true);
        std::cout << "设置 (3,0) 可达后:" << std::endl;
        for (size_t i = 0; i < 4; ++i) {
            for (size_t j = 0; j < 4; ++j) {
                std::cout << (matrix[i].test(j) ? "1" : "0") << " ";
            }
            std::cout << std::endl;
        }
        
        // 测试从邻接矩阵构造
        std::cout << "\n=== 测试从邻接矩阵构造 ===" << std::endl;
        std::vector<tg::dynamic_bitset<size_t>> adj(3, tg::dynamic_bitset<size_t>(3));
        adj[0].set(1);
        adj[1].set(2);
        adj[2].set(0);
        
        tg::ReachMatrix matrix2(adj);
        std::cout << "环形图的可达矩阵:" << std::endl;
        for (size_t i = 0; i < 3; ++i) {
            for (size_t j = 0; j < 3; ++j) {
                std::cout << (matrix2[i].test(j) ? "1" : "0") << " ";
            }
            std::cout << std::endl;
        }
        
        // 测试 reset 功能
        std::cout << "\n=== 测试 reset 功能 ===" << std::endl;
        matrix.reset();
        std::cout << "重置后的矩阵（只有自环）:" << std::endl;
        for (size_t i = 0; i < 4; ++i) {
            for (size_t j = 0; j < 4; ++j) {
                std::cout << (matrix[i].test(j) ? "1" : "0") << " ";
            }
            std::cout << std::endl;
        }
        
        std::cout << "\n所有测试完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
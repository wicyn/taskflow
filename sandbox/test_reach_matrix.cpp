#include <cassert>
#include <iostream>
#include "../include/tg/reach_matrix.hpp"

int main() {
    tg::ReachMatrix rm(4);
    // Initially only self-loops
    for (size_t i = 0; i < rm.count(); ++i) {
        for (size_t j = 0; j < rm.count(); ++j) {
            if (i == j) assert(rm[i][j]);
            else assert(!rm.reachable(i, j));
        }
    }
    // add edges 0->1, 1->2, 2->3 -> should make 0 reach 3
    rm.add_edge(0,1);
    rm.add_edge(1,2);
    rm.add_edge(2,3);
    assert(rm.reachable(0,3));

    // [][] write access: set 3->0 directly
    rm[3][0] = true;
    assert(rm.reachable(3,0));

    // swap nodes 0 and 3
    rm.swap(0,3);
    assert(rm.reachable(0,3));

    // push and remove back
    rm.push_back();
    assert(rm.count() == 5);
    assert(rm[4][4]);
    rm.remove_back();
    assert(rm.count() == 4);

    // reset to identity
    rm.reset();
    for (size_t i = 0; i < rm.count(); ++i) {
        for (size_t j = 0; j < rm.count(); ++j) {
            if (i == j) assert(rm[i][j]);
            else assert(!rm.reachable(i, j));
        }
    }

    std::cout << "OK\n";
    return 0;
}

#include <cassert>
#include <iostream>
#include <pthread.h>
#include "cpu.hpp"


void test_bc_reg() {
    CPU testCPU;
    testCPU.B = 0x00;
    testCPU.C = 0x00;
    testCPU.setBC(0xaabb);
    assert(testCPU.B == 0xaa);
    assert(testCPU.C == 0xbb);
    return;
}

void test_de_reg() {
    CPU testCPU;
    testCPU.D = 0x00;
    testCPU.E = 0x00;
    testCPU.setDE(0xccdd);
    assert(testCPU.D == 0xcc);
    assert(testCPU.E == 0xdd);
    return;
}


int main() {
    test_bc_reg();
    test_de_reg();
    return 0;
}
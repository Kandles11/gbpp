#include <cassert>
#include <iostream>
#include <pthread.h>

struct Cpu {
    uint16_t SP;
    uint16_t PC;
    uint8_t A, B, C, D, E, H, L;
    bool zero, sub, halfcarry, carry; //flags

    uint16_t get_word_reg(uint8_t &hi, uint8_t &lo) {
        uint16_t wordreg = hi;
        wordreg = (hi << 8) | lo;
        return wordreg;
    }

    void set_word_reg(uint16_t val, uint8_t &hi, uint8_t &lo) {
        hi = (0xFF00 & val) >> 8;
        lo = 0x00FF & val;
        return;   
    }

    uint8_t getF() {
        uint8_t flags = 0x00;
        flags = (zero << 7) | (sub << 6) | (halfcarry << 5) | (carry << 4);
        return flags;
    }

    uint16_t getAF() {
        uint16_t af = A;
        af = (A << 8) | getF();
        return af;
    }

    void setBC(uint16_t val) {
        set_word_reg(val, B, C);
    }

    void setDE(uint16_t val) {
        set_word_reg(val, D, E);
    }

    void setHL(uint16_t val) {
        set_word_reg(val, H, L);
    }

    uint16_t getBC() {
        return get_word_reg(B, C);
    }

    uint16_t getDE() {
        return get_word_reg(D, E);
    }

    uint16_t getHL() {
        return get_word_reg(H, L);
    }
};


void test_bc_reg() {
    Cpu testCPU;
    testCPU.B = 0x00;
    testCPU.C = 0x00;
    testCPU.setBC(0xaabb);
    assert(testCPU.B == 0xaa);
    assert(testCPU.C == 0xbb);
    return;
}

void test_de_reg() {
    Cpu testCPU;
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
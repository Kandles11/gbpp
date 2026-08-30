#include <iostream>

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
        lo = (0xFF00 & val) >> 8;
        hi = 0x00FF & val;
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

int main() {
    Cpu myCpu;
    myCpu.SP = 0x4040;
    myCpu.PC = 0xABAB;

    myCpu.A = 0xAb;
    myCpu.zero = 1;
    myCpu.sub = 0;
    myCpu.halfcarry = 1;
    myCpu.carry = 0;
    uint16_t af = myCpu.getAF();
    return 0;
}

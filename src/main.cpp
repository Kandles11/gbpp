#include <iostream>

struct Cpu {
    uint16_t SP;
    uint16_t PC;
    uint8_t A, B, C, D, E, H, L;

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

    uint16_t value = 0xAABB;
    myCpu.setBC(value);
    uint16_t bcval = myCpu.getBC();
    return 0;
}

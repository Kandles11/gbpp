#include <cstdint>
#include <iostream>

struct CPU {
    uint16_t SP;
    uint16_t PC;
    uint8_t A, B, C, D, E, H, L;
    bool zero, sub, halfcarry, carry; //flags

    uint16_t get_word_reg(const uint8_t &hi, const uint8_t &lo);
    void set_word_reg(const uint16_t value, uint8_t &hi, uint8_t &lo);    
    uint8_t getF();
    uint16_t getAF();
    void setBC(uint16_t);
    void setDE(uint16_t);
    void setHL(uint16_t);
    uint16_t getBC();
    uint16_t getDE();
    uint16_t getHL();
};
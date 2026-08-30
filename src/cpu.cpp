#include "cpu.hpp"
#include <iostream>


uint16_t CPU::get_word_reg(const uint8_t &hi, const uint8_t &lo) {
    uint16_t wordreg = hi;
    wordreg = (hi << 8) | lo;
    return wordreg;
}

void CPU::set_word_reg(const uint16_t val, uint8_t &hi, uint8_t &lo) {
    hi = (0xFF00 & val) >> 8;
    lo = 0x00FF & val;
    return;   
}

uint8_t CPU::getF() {
    uint8_t flags = 0x00;
    flags = (zero << 7) | (sub << 6) | (halfcarry << 5) | (carry << 4);
    return flags;
}

uint16_t CPU::getAF() {
    uint16_t af = A;
    af = (A << 8) | getF();
    return af;
}

void CPU::setBC(uint16_t val) {
    set_word_reg(val, B, C);
}

void CPU::setDE(uint16_t val) {
    set_word_reg(val, D, E);
}

void CPU::setHL(uint16_t val) {
    set_word_reg(val, H, L);
}

uint16_t CPU::getBC() {
    return get_word_reg(B, C);
}

uint16_t CPU::getDE() {
    return get_word_reg(D, E);
}

uint16_t CPU::getHL() {
    return get_word_reg(H, L);
}
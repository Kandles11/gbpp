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

uint8_t* CPU::decodeToRegister(uint8_t code) {
    switch(code) {
        case 0:
            return &B;
            break;
        case 1:
            return &C;
            break;
        case 2:
            return &D;
            break;
        case 3:
            return &E;
            break;
        case 4:
            return &H;
            break;
        case 5:
            return &L;
            break;
        case 6:
            std::cout << "UNIMPLEMENTED TODAY" << std::endl;
            break;
        case 7:
            return &A;
            break;
        default:
            return nullptr;
    }
    return nullptr;
}

uint8_t CPU::fetchByte(Memory &mem) {
    uint8_t data = mem.data[PC];
    PC++;
    return data;
}

void CPU::execute(int cycles, Memory &mem) {
    uint8_t instruction = fetchByte(mem);
    if ((instruction & 0xC7) == 0x06)
    {
        //ld r8, imm8
        int code = (instruction >> 3) & 0x07;
        uint8_t* dest = decodeToRegister(code);
        uint8_t val = fetchByte(mem);
        *dest = val;
    }
    return; 
}
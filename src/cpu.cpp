#include "cpu.hpp"
#include <cstddef>
#include <cstdint>
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

void CPU::setWordRegFromCode(uint8_t code, uint16_t val) {
    switch(code) { 
        case 0:
            setBC(val);
            break;
        case 1:
            setDE(val);
            break;
        case 2: 
            setHL(val);
            break;
        case 3:
            SP = val;
            break;
        default:
            break; 
    }
}

uint16_t CPU::getWordRegFromCode(uint8_t code) {
    uint16_t val;
    switch(code) { 
        case 0:
            val = getBC();
            break;
        case 1:
            val = getDE();
            break;
        case 2: 
            val = getHL();
            break;
        case 3:
            val = SP;
            break;
        default:
            val = NULL;
            break; 
    }
    return val;
}

uint8_t CPU::fetchByte(Memory &mem) {
    uint8_t data = mem.data[PC];
    PC++;
    return data;
}

uint16_t CPU::fetchWord(Memory &mem) {
    uint8_t lo = mem.data[PC];
    PC++;
    uint8_t hi = mem.data[PC];
    PC++;
    uint16_t data = (hi << 8) | lo;
    return data;
}

void CPU::execute(int ticks, Memory &mem) {
    while (ticks > 0){
        uint8_t instruction = fetchByte(mem);
        if (instruction == 0x00){
            //nop
            ticks -= 4;
        }
        else if ((instruction & 0xCF) == 0x01) {
            //ld r16, imm16	
            int code = (instruction >> 4) & 0x03;
            uint16_t val = fetchWord(mem);
            setWordRegFromCode(code, val);
            ticks -= 12;
        }
        else if ((instruction & 0xCF) == 0x02) {
            //ld [r16mem], a
            int code = (instruction >> 4) & 0x03;
            uint16_t address = getWordRegFromCode(code);
            mem.data[address] = A;
            ticks -= 8;
        }
        else if ((instruction & 0xCF) == 0x0A) {
            // ld a, [r16mem]
            int code = (instruction >> 4) & 0x03;
            uint16_t address = getWordRegFromCode(code);
            A = mem.data[address];
            ticks -= 8;
        }
        else if (instruction == 0x08) {
            //ld [imm16], sp
            uint16_t address = fetchWord(mem);
            mem.data[address] = SP;
        }
        else if ((instruction & 0xC7) == 0x06)
        {
            //ld r8, imm8
            int code = (instruction >> 3) & 0x07;
            uint8_t* dest = decodeToRegister(code);
            uint8_t val = fetchByte(mem);
            *dest = val;
            ticks -= 4;
        }
    }
    return;
}
#include "cpu.hpp"
#include <cstddef>
#include <cstdint>
#include <bit>
#include <iostream>
#include <sys/syslimits.h>

CPU::CPU() {
    SP =PC = 0;
    A = B = C = D = E = H = L = 0;
    zero = sub = halfcarry = carry = 0;
}


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
        case 7:
            return &A;
            break;
        default:
            throw std::runtime_error("INVALID REGISTER CODE"); 
    }
    return nullptr;
}

uint8_t CPU::getRegFromCode(uint8_t code, Memory mem)
{
    if (code == 6) {
        // [HL] case
        uint16_t addr = getHL();
        return mem.data[addr];
    }
    else {
        uint8_t* reg = decodeToRegister(code);
        return *reg;
    }
}

void CPU::setRegFromCode(uint8_t code, uint8_t val, Memory mem)
{
    if (code == 6) {
        // [HL] case
        uint16_t addr = getHL();
        mem.data[addr] = val;
    }
    else {
        uint8_t* reg = decodeToRegister(code);
        *reg = val;
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
            std::cout << "Invalid register code given" << std::endl;
            break; 
    }
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

void CPU::outputToSerial(Memory &mem)
{
    char val = mem.data[0xFF01];
    uint8_t SCData = mem.data[0xFF02];
    bool transferEnable = SCData >> 7;
    enum class ClockSelect { External, Internal};
    ClockSelect cs = ClockSelect::External;
    if ((SCData & 0x01) == 0) {
        cs = ClockSelect::External;
    } else { 
        cs = ClockSelect::Internal;
    }
    if (transferEnable == 1)
    {
        std::cout << val << std::flush;
        mem.data[0xFF02] = 0x00;
    }

}

void CPU::execute(int ticks, Memory &mem) {
    while (true){
        uint8_t instruction = fetchByte(mem);
        if (instruction == 0x00){
            //nop
            PC++;
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
            ticks -= 20;
        }
        else if ((instruction & 0xCF) == 0x03) {
            //inc r16
            int code = (instruction >> 4) & 0x03;
            uint16_t val = getWordRegFromCode(code);
            setWordRegFromCode(code, ++val);
            ticks -= 8;
        }
        else if ((instruction & 0xCF) == 0x0B) {
            //dec r16
            int code = (instruction >> 4) & 0x03;
            uint16_t val = getWordRegFromCode(code);
            setWordRegFromCode(code, --val);
            ticks -= 8; 
        }
        else if ((instruction & 0xCF) == 0x09) {
            // TODO HANDLE FLAGS HERE
            //add hl, r16
            int code = (instruction >> 4) & 0x03;
            uint16_t val = getWordRegFromCode(code);
            setHL(getHL() + val);
            ticks -= 8;
        }
        else if ((instruction & 0xC7) == 0x04) {
            // TODO HANDLE FLAGS HERE
            //inc r8
            int code = (instruction >> 3) & 0x07;
            uint8_t val = getRegFromCode(code, mem);
            setRegFromCode(code, val+1, mem);
            ticks -= 4;
        }
        else if ((instruction & 0xC7) == 0x05) {
            //dec r8
            //TODO HANDLE FLAGS HERE
            int code = (instruction >> 3) & 0x07;
            uint8_t val = getRegFromCode(code, mem);
            setRegFromCode(code, val-1, mem);
            ticks -= 4;
        }
        else if ((instruction & 0xC7) == 0x06)
        {
            //ld r8, imm8
            int code = (instruction >> 3) & 0x07;
            uint8_t val = fetchByte(mem);
            setRegFromCode(code, val, mem);
            ticks -= 4;
        }
        else if (instruction == 0x07)
        {
            // rlca
            uint8_t val = A;
            uint8_t res = val << 1;
            uint8_t mask = (val >> 7);
            A = res | mask;
            carry = val >> 7;
            ticks -= 4;
        }
        else if(instruction == 0x0F)
        {
            //rrca
            uint8_t val = A;
            uint8_t res = val >> 1;
            uint8_t mask = (val << 7);
            A = res | mask;
            carry = val & 0b00000001;
            ticks -= 4;
        }
        else if (instruction == 0x17) {
            //rla
            uint8_t val = A;
            uint8_t res = val << 1;
            uint8_t mask = carry;
            A = res | mask;
            carry = val >> 7;
            ticks -= 4;
        }
        else if (instruction == 0x1F) {
            //rra
            uint8_t val = A;
            uint8_t res = val >> 1;
            uint8_t mask = carry << 7;
            A = res | mask;
            carry = val & 0b00000001;
            ticks -= 4;

        }
        else if (instruction == 0x37) {
            // scf
            carry = 1;
            ticks -= 4;
        }
        else if (instruction == 0x3F)
        {
            // ccf
            carry = !carry;
            ticks -= 4;
        }
        else if (instruction == 0x18)
        {
            //jr imm8
            int8_t jumpValue = fetchByte(mem);
            PC += jumpValue;
            ticks -= 12;
        }
        else if ((instruction & 0b11100111) == 0x20)
        {
            //jr cond, imm8
            uint8_t condition = (instruction & 0b00011000) >> 3;
            int8_t jumpValue = fetchByte(mem);
            switch(condition){
                case 0:
                    //nz
                    if (!zero) {
                        PC += jumpValue;
                        ticks -= 12;
                    } else {
                        ticks -= 8;
                    }
                    break;
                case 1:
                    //z
                    if (zero) {
                        PC += jumpValue;
                        ticks -= 12;
                    } else {
                        ticks -= 8;
                    }
                    break;
                case 2:
                    //nc
                    if (!carry) {
                        PC += jumpValue;
                        ticks -= 12;
                    } else {
                        ticks -= 8;
                    }
                    break;
                case 3:
                    //c
                    if (carry) {
                        PC += jumpValue;
                        ticks -= 12;
                    } else {
                        ticks -= 8;
                    }
                    break;
                default:
                    std::cout << "invalid condition" << std::endl;
                    break;
            }
        }
        else if (instruction == 0x10)
        {
            //stop
            // TODO: needs more nuance according to the docs
            ticks -= 4;
            break;
        }
        else if ((instruction & 0b11000000) == 0x40) 
        {
            //ld r8, r8
            uint8_t dest_code = (instruction & 0b00111000) >> 3;
            uint8_t src_code = (instruction & 0b00000111);
            uint8_t src_val = getRegFromCode(src_code, mem);
            setRegFromCode(dest_code, src_val, mem);
            ticks -= 4;
        }
        else if ((instruction & 0b11111000) == 0x80)
        {
            //add a, r8
            uint8_t src_code = (instruction & 0b00111000) >> 3;
            uint8_t src_val = getRegFromCode(src_code, mem);
            halfcarry = (A & 0x0F) + (src_val & 0x0F) > 0x0F;
            uint16_t fullVal = A + src_val;
            if (fullVal > 0xFF) { carry = 1;} else { carry = 0;}
            A = A + src_val;
            if (A == 0) { zero = 1;} else { zero = 0;}
            sub = 0;
            ticks -= 4;
        }
        else {
            std::string msg;
            msg = "unknown instruction given: " + std::to_string(static_cast<char>(instruction));
            throw std::runtime_error(msg); 
        }
        outputToSerial(mem);
    }
    return;
}


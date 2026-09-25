#include "cpu.hpp"
#include <codecvt>
#include <cstddef>
#include <cstdint>
#include <bit>
#include <iostream>
#include <fstream> 
#include <sys/syslimits.h>
#include <sstream>   // std::ostringstream
#include <iomanip>   // std::setw, std::setfill

CPU::CPU() {
    PC = 0;
    SP = 0xFFFE;
    A = B = C = D = E = H = L = 0;
    zero = sub = halfcarry = carry = 0;
    interruptMasterEnable = 0;
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

void CPU::setF(uint8_t val) {
    zero      = (val >> 7) & 1;
    sub       = (val >> 6) & 1;
    halfcarry = (val >> 5) & 1;
    carry     = (val >> 4) & 1;
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

void CPU::setAF(uint16_t val) {
    uint8_t hi = (0xFF00 & val) >> 8;
    uint8_t lo = 0x00FF & val;
    A = hi;
    setF(lo);
    return;
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
        return mem.readMem(addr);
    }
    else {
        uint8_t* reg = decodeToRegister(code);
        return *reg;
    }
}

void CPU::setRegFromCode(uint8_t code, uint8_t val, Memory& mem)
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

uint16_t CPU::getStackRegFromCode(uint8_t code) {
    uint16_t val;
    switch (code) {
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
            val = getAF();
            break;
        default:
            val= NULL;
            break;
    }
    return val;
}

void CPU::setStackRegFromCode(uint8_t code, uint16_t val) {
    switch (code) {
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
            setAF(val);
            break;
        default:
            std::cout << "Invalid register code given" << std::endl;
            break;
    }
}

uint8_t CPU::fetchByte(Memory &mem) {
    uint8_t data = mem.readMem(PC);
    PC++;
    return data;
}

uint16_t CPU::fetchWord(Memory &mem) {
    uint8_t lo = mem.readMem(PC);
    PC++;
    uint8_t hi = mem.readMem(PC);
    PC++;
    uint16_t data = (hi << 8) | lo;
    return data;
}

void CPU::outputToSerial(Memory &mem)
{
    char val = mem.readMem(0xFF01);
    uint8_t SCData = mem.readMem(0xFF02);
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

void CPU::logStatus(std::ofstream* logfile, Memory& mem) {
    *logfile << "A:" << std::hex << std::uppercase 
            << std::setw(2) << std::setfill('0') << +A 
            << " F:" << std::setw(2) << +getF() 
            << " B:" << std::setw(2) << +B 
            << " C:" << std::setw(2) << +C 
            << " D:" << std::setw(2) << +D 
            << " E:" << std::setw(2) << +E 
            << " H:" << std::setw(2) << +H 
            << " L:" << std::setw(2) << +L 
            << " SP:" << std::setw(4) << +SP 
            << " PC:" << std::setw(4) << +PC 
            << " PCMEM:" << std::setw(2) << +mem.readMem(PC) 
            << "," << std::setw(2) << +mem.readMem(PC+1) 
            << "," << std::setw(2) << +mem.readMem(PC+2) 
            << "," << std::setw(2) << +mem.readMem(PC+3) 
            << std::endl;
}

void CPU::execute(int ticks, Memory &mem, std::ofstream *logfile, bool unlimited) {
    while (ticks > 0 or unlimited){
        if (logfile) {
            logStatus(logfile, mem);
        }
        uint8_t instruction = fetchByte(mem);
        if (instruction == 0xCB) {
            uint8_t nextInstr = fetchByte(mem);
            if ((nextInstr & 0b11111000) == 0x38)
            {
                uint8_t code = (nextInstr & 0b00000111);
                uint8_t val = getRegFromCode(code, mem);
                uint8_t shifted = val >> 1;
                carry = val & 1;
                setRegFromCode(code, shifted, mem);
                if (shifted == 0) { zero = 1;} else {zero = 0;}
                sub = 0;
                halfcarry =0;
                ticks -= 8;
            }
            else if ((nextInstr & 0b11111000) == 0x18)
            {
                uint8_t code = (nextInstr & 0b00000111);
                uint8_t val = getRegFromCode(code, mem);
                uint8_t oldCarry = carry;
                uint8_t shifted = val >> 1;
                carry = val & 1;
                shifted = shifted | (oldCarry << 7);
                setRegFromCode(code, shifted, mem);
                if (shifted == 0) { zero = 1;} else {zero = 0;}
                sub =0 ;    
                halfcarry =0;
                ticks -= 8;
            }
        }
        else if (instruction == 0x00){
            //nop
            ticks -= 4;
        }
        else if (instruction == 0xF9)
        {
            uint16_t val = getHL();
            SP = val;
            ticks -= 8;
        }
        else if (instruction == 0xfa)
        {
            // ld a, [imm16]
            uint16_t addr = fetchWord(mem);
            A = mem.readMem(addr);
            ticks -= 16;
        }
        else if (instruction == 0xe8)
        {
            // add sp, imm8
            uint8_t raw = fetchByte(mem);
            int8_t val = (int8_t)raw;
            halfcarry = ((SP & 0x0F) + (raw & 0x0F)) > 0x0F;  
            carry = ((SP & 0xFF) + (raw & 0xFF)) > 0xFF;  
            SP = SP + val;
            zero = 0;
            sub = 0;
            ticks -= 16;
        }
        else if (instruction == 0xf8)
        {
            // ld hl, sp+e8
            uint8_t raw = fetchByte(mem);
            int8_t val = (int8_t)raw;
            halfcarry = ((SP & 0x0F) + (raw & 0x0F)) > 0x0F;
            carry = ((SP & 0xFF) + (raw & 0xFF)) > 0xFF;  
            setHL(SP + val);
            zero =0;
            sub = 0;
            ticks -= 12;
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
            if (code == 2 | code == 3 ) {
                //hl +- case
                uint16_t address = getHL();
                mem.data[address] = A;
                if (code == 2) {setHL(address + 1);} else {setHL(address - 1);}
            } else {
                uint16_t address = getWordRegFromCode(code);
                mem.data[address] = A;
            }
            ticks -= 8;
        }
        else if ((instruction & 0xCF) == 0x0A) {
            // ld a, [r16mem]
            int code = (instruction >> 4) & 0x03;
            if (code == 2 | code == 3 ) {
                //hl +- case
                uint16_t address = getHL();
                A = mem.readMem(address);
                if (code == 2) {setHL(address + 1);} else {setHL(address - 1);}
            } else {
                uint16_t address = getWordRegFromCode(code);
                A = mem.readMem(address);
            }
            ticks -= 8;
        }
        else if (instruction == 0x08) {
            //ld [imm16], sp
            uint16_t address = fetchWord(mem);
            mem.data[address] = (0x00FF & SP);
            mem.data[address+1] = SP >> 8;
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
            //add hl, r16
            int code = (instruction >> 4) & 0x03;
            uint16_t val = getWordRegFromCode(code);
            uint32_t fullval = val + getHL();
            halfcarry = ((getHL() & 0x0FFF) + (val & 0x0FFF)) > 0x0FFF;
            setHL(getHL() + val);
            sub =0;
            if (fullval > 0xFFFF) { carry = 1; } else {carry = 0;} 
            ticks -= 8;
        }
        else if ((instruction & 0xC7) == 0x04) {
            //inc r8
            int code = (instruction >> 3) & 0x07;
            uint8_t val = getRegFromCode(code, mem);
            uint8_t newVal = val + 1;
            setRegFromCode(code, newVal, mem);
            if (newVal == 0) { zero = 1; } else { zero = 0;} 
            sub = 0;
            if ((val & 0x0F) == 0x0F) { halfcarry = 1; } else { halfcarry = 0;}
            ticks -= 4;
        }
        else if ((instruction & 0xC7) == 0x05) {
            //dec r8
            int code = (instruction >> 3) & 0x07;
            uint8_t val = getRegFromCode(code, mem);
            setRegFromCode(code, val-1, mem);
            if (val - 1 == 0) { zero = 1;} else {zero = 0;}
            sub = 1;
            if ((val & 0x0F) == 0x00) { halfcarry = 1;} else {halfcarry =0;}
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
            zero = 0;
            sub = 0;
            halfcarry =0;
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
            uint8_t src_code = (instruction & 0b00000111) >> 3;
            uint8_t src_val = getRegFromCode(src_code, mem);
            halfcarry = (A & 0x0F) + (src_val & 0x0F) > 0x0F;
            uint16_t fullVal = A + src_val;
            if (fullVal > 0xFF) { carry = 1;} else { carry = 0;}
            A = A + src_val;
            if (A == 0) { zero = 1;} else { zero = 0;}
            sub = 0;
            ticks -= 4;
        }
        else if ((instruction & 0b11111000) == 0x88)
        {
            //adc a, r8
            uint8_t src_code = (instruction & 0b00000111) >> 3;
            uint8_t src_val = getRegFromCode(src_code, mem);
            halfcarry = (A & 0x0F) + (src_val & 0x0F) + carry > 0x0F;
            uint16_t fullVal = A + src_val + carry;
            A = A + src_val + carry;
            if (fullVal > 0xFF) { carry = 1;} else { carry = 0;}
            if (A == 0) { zero = 1;} else { zero = 0;}
            sub = 0;
            ticks -= 4;
        }
        else if (instruction == 0xCE)
        {
            //adc a, imm8
            uint8_t val = fetchByte(mem);
            halfcarry = (A & 0x0F) + (val & 0x0F) + carry > 0x0F;
            uint16_t fullVal = A + val + carry;
            A = A + val + carry;
            if (fullVal > 0xFF) { carry = 1;} else { carry = 0;}
            if (A == 0) { zero = 1;} else { zero = 0;}
            sub = 0;
            ticks -= 8;
        }
        else if ((instruction & 0b11100111) == 0xC4)
        {
            //call cond, imm16
            uint8_t condition = (instruction & 0b00011000) >> 3;
            uint16_t dest = fetchWord(mem);
            switch(condition){
                case 0:
                    //nz
                    if (!zero) {
                        SP -= 1;
                        mem.data[SP] = PC >> 8;
                        SP -= 1;
                        mem.data[SP] = PC & 0xFF;
                        PC = dest;
                        ticks -= 24;
                    } else {
                        ticks -= 12;
                    }
                    break;
                case 1:
                    //z
                    if (zero) {
                        SP -= 1;
                        mem.data[SP] = PC >> 8;
                        SP -= 1;
                        mem.data[SP] = PC & 0xFF;
                        PC = dest;
                        ticks -= 24;
                    } else {
                        ticks -= 12;
                    }
                    break;
                case 2:
                    //nc
                    if (!carry) {
                        SP -= 1;
                        mem.data[SP] = PC >> 8;
                        SP -= 1;
                        mem.data[SP] = PC & 0xFF;
                        PC = dest;
                        ticks -= 24;
                    } else {
                        ticks -= 12;
                    }
                    break;
                case 3:
                    //c
                    if (carry) {
                        SP -= 1;
                        mem.data[SP] = PC >> 8;
                        SP -= 1;
                        mem.data[SP] = PC & 0xFF;
                        PC = dest;
                        ticks -= 24;
                    } else {
                        ticks -= 12;
                    }
                    break;
                default:
                    std::cout << "invalid condition" << std::endl;
                    break;
            }
        }
        else if (instruction == 0xC6)
        {
            //add a, imm8
            uint8_t val = fetchByte(mem);
            halfcarry = (A & 0x0F) + (val & 0x0F) > 0x0F;
            uint16_t fullVal = A + val;
            A = A + val;
            if (fullVal > 0xFF) { carry = 1;} else { carry = 0;}
            if (A == 0) { zero = 1;} else { zero = 0;}
            sub = 0;
            ticks -= 8;
        }
        else if (instruction == 0xCE)
        {
            //adc a, imm8
            uint8_t val = fetchByte(mem);
            halfcarry = (A & 0x0F) + (val & 0x0F) + carry > 0x0F;
            uint16_t fullVal = A + val + carry;
            A = A + val + carry;
            if (fullVal > 0xFF) { carry = 1;} else { carry = 0;}
            if (A == 0) { zero = 1;} else { zero = 0;}
            sub = 0;
            ticks -= 8;
        }
        else if (instruction == 0xD6)
        {
            //sub a, imm8
            uint8_t val = fetchByte(mem);
            halfcarry = (A & 0x0F) < (val & 0x0F);
            carry = val > A;
            A = A - val;
            if (A == 0) { zero = 1;} else { zero = 0;}
            sub = 1;
            ticks -= 8;
        }
        else if (instruction == 0xDE)
        {
            //sbc a, imm8
            uint8_t val = fetchByte(mem);
            bool oldCarry = carry;
            halfcarry = (A & 0x0F) < (val & 0x0F) + oldCarry;
            carry = val + oldCarry > A;
            A = A - (val + oldCarry);
            if (A == 0) { zero = 1;} else { zero = 0;}
            sub = 1;
            ticks -= 8;
        }
        else if (instruction == 0xE6)
        {
            //and a, imm8
            uint8_t val = fetchByte(mem);
            A = A & val;
            if (A == 0) { zero = 1;} else { zero = 0;}
            sub = 0;
            halfcarry = 1;
            carry = 0;
            ticks -= 8;
        }
        else if (instruction == 0xEE)
        {
            //xor a, imm8
            uint8_t val = fetchByte(mem);
            A = A ^ val;
            if (A == 0) { zero = 1;} else { zero = 0;}
            sub = 0;
            halfcarry = 0;
            carry = 0;
            ticks -= 8;
        }
        else if (instruction == 0xF6)
        {
            //or a, imm8
            uint8_t val = fetchByte(mem);
            A = A | val;
            if (A == 0) { zero = 1;} else { zero = 0;}
            sub = 0;
            halfcarry = 0;
            carry = 0;
            ticks -= 8;
        }
        else if (instruction == 0xFE)
        {
            //cp a, imm8
            uint8_t val = fetchByte(mem);
            uint8_t result = A - val;
            if (result == 0) { zero = 1;} else { zero = 0;}
            sub = 1;
            halfcarry = (A & 0x0F) < (val & 0x0F);
            carry = val > A;
            ticks -= 8;
        }
        else if (instruction == 0xE9)
        {
            //jp hl
            uint16_t addr = getHL();
            PC = addr;
            ticks -= 4;
        }
        else if (instruction == 0xc3)
        {
            //jp imm16
            uint16_t addr = fetchWord(mem);
            PC = addr;
            ticks -= 16;
        }
        else if (instruction == 0xF3)
        {
            // di
            interruptMasterEnable = 0;
            ticks -= 4;
        }
        else if (instruction == 0xea)
        {
            // ld [imm16] , a
            uint16_t addr = fetchWord(mem);
            mem.data[addr] = A;
            ticks -= 16;
        }
        else if (instruction == 0xe0)
        {
            // ldh [imm8], a
            uint8_t lo = fetchByte(mem);
            uint16_t addr = 0xFF00 | lo;
            mem.data[addr] = A;
            ticks -= 12;
        }
        else if (instruction == 0xf0)
        {
            // ldh a, [imm8]
            uint8_t lo = fetchByte(mem);
            uint16_t addr = 0xFF00 | lo;
            A = mem.readMem(addr);
            ticks -= 12;

        }
        else if (instruction == 0xcd)
        {
            // call imm16
            uint16_t dest = fetchWord(mem);
            SP -= 1;
            mem.data[SP] = PC >> 8;
            SP -= 1;
            mem.data[SP] = PC & 0xFF;
            PC = dest;
            ticks -= 24;
        }
        else if (instruction == 0xc9)
        {
            // ret
            uint8_t lo = mem.readMem(SP);
            SP += 1;
            uint16_t hi = mem.readMem(SP);
            SP += 1;
            uint16_t addr = (hi << 8) | lo;
            PC = addr;
            ticks -= 16;
        }
        else if ((instruction & 0b11100111) == 0xC0)
        {
            // ret cond
            uint8_t condition = (instruction & 0b00011000) >> 3;
            switch(condition){
                case 0:
                    //nz
                    if (!zero) {
                        uint8_t lo = mem.readMem(SP);
                        SP += 1;
                        uint16_t hi = mem.readMem(SP);
                        SP += 1;
                        uint16_t addr = (hi << 8) | lo;
                        PC = addr;
                        ticks -= 20;
                    } else {
                        ticks -= 8;
                    }
                    break;
                case 1:
                    //z
                    if (zero) {
                        uint8_t lo = mem.readMem(SP);
                        SP += 1;
                        uint16_t hi = mem.readMem(SP);
                        SP += 1;
                        uint16_t addr = (hi << 8) | lo;
                        PC = addr;
                        ticks -= 20;
                    } else {
                        ticks -= 8;
                    }
                    break;
                case 2:
                    //nc
                    if (!carry) {
                        uint8_t lo = mem.readMem(SP);
                        SP += 1;
                        uint16_t hi = mem.readMem(SP);
                        SP += 1;
                        uint16_t addr = (hi << 8) | lo;
                        PC = addr;
                        ticks -= 20;
                    } else {
                        ticks -= 8;
                    }
                    break;
                case 3:
                    //c
                    if (carry) {
                        uint8_t lo = mem.readMem(SP);
                        SP += 1;
                        uint16_t hi = mem.readMem(SP);
                        SP += 1;
                        uint16_t addr = (hi << 8) | lo;
                        PC = addr;
                        ticks -= 20;
                    } else {
                        ticks -= 8;
                    }
                    break;
                default:
                    std::cout << "invalid condition" << std::endl;
                    break;
            }

        }
        else if ((instruction & 0b11001111) == 0xC5)
        {
            // push r16stk
            uint8_t code = (instruction & 0b00110000) >> 4;
            uint16_t val = getStackRegFromCode(code);
            SP -= 1;
            mem.data[SP] = val >> 8;
            SP -= 1;
            mem.data[SP] = val & 0x00FF;
            ticks -= 16;
        }
        else if ((instruction & 0b11000001) == 0xc1)
        {
            //pop r16stk
            uint8_t code = (instruction & 0b00110000) >> 4;
            uint8_t lo = mem.readMem(SP);
            SP += 1;
            uint16_t hi = mem.readMem(SP);
            SP += 1;
            uint16_t val = (hi << 8) | lo;
            setStackRegFromCode(code, val);
            ticks -= 12;
        }
        else if ((instruction & 0b11111000) == 0xB0) {
            //or a, r8
            uint8_t code = (instruction & 0b00000111);
            uint8_t val = getRegFromCode(code, mem);
            A = A | val;
            if (A == 0) { zero = 1; } else { zero = 0;}
            sub = 0;
            halfcarry = 0;
            carry = 0;
            ticks -=4;
        }
        else if ((instruction & 0b11111000) == 0xA8)
        {
            // xor a, r8
            uint8_t code = (instruction & 0b00000111);
            uint8_t val = getRegFromCode(code, mem);
            A = A ^ val;
            if (A == 0) { zero = 1; } else { zero = 0;}
            sub = 0;
            halfcarry = 0;
            carry = 0;
            ticks -=4;
        }

        else {
            std::ostringstream ss;
            ss << "unknown instruction given: 0x" << std::hex << std::setw(2)
            << std::setfill('0') << int(instruction);
            throw std::runtime_error(ss.str()); 
        }
        outputToSerial(mem);
    }
    return;
}



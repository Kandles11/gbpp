#include <cassert>
#include <iostream>
#include <pthread.h>
#include "cpu.hpp"


int main() {
    CPU cpu;
    Memory mem;
    mem.clear();
    //set C and B registers
    mem.data[0x0000] = 0b00001110;
    mem.data[0x0001] = 0xCC;
    mem.data[0x0002] = 0b00000110;
    mem.data[0x0003] = 0xBB;

    //set DE
    mem.data[0x0004] = 0b00010001;
    mem.data[0x0005] = 0xFF;
    mem.data[0x0006] = 0xAA;

    cpu.execute(16, mem);
    return 0;
}
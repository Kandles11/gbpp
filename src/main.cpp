#include <cassert>
#include <iostream>
#include <pthread.h>
#include "cpu.hpp"


int main() {
    CPU cpu;
    Memory mem;
    mem.clear();
    mem.data[0x0000] = 0b00001110;
    mem.data[0x0001] = 0xBB;
    
    cpu.execute(5, mem);
    return 0;
}
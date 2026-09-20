#include <cassert>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include "cpu.hpp"


int main() {
    CPU cpu;
    Memory mem;
    mem.clear();
    mem.data[0x0000] = 0b00000111;
    cpu.A = 0b10101010;

    // std::ifstream GBinary("test.gb");
    // GBinary.read(reinterpret_cast<char*>(mem.data), 30);

    cpu.execute(100, mem);
    return 0;
}
#include <cassert>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include "cpu.hpp"


int main() {
    CPU cpu;
    Memory mem;
    mem.clear();

    std::ifstream GBinary("test.gb");
    GBinary.read(reinterpret_cast<char*>(mem.data), 30);

    cpu.execute(100, mem);
    return 0;
}
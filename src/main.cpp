#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include "cpu.hpp"


void setPostBootRomState(CPU &cpu)
{
    cpu.A = 0x01; 
    cpu.zero = 1;
    cpu.sub = 0;
    cpu.halfcarry = 1;
    cpu.carry = 1;
    cpu. B = 0x00;
    cpu.C = 0x13;
    cpu.D = 0x00; 
    cpu.E = 0xD8;
    cpu.H = 0x01; 
    cpu.L = 0x4D;
    cpu.SP = 0xFFFE;
    cpu.PC = 0x0100;
}


int main() {
    CPU cpu;
    Memory mem;
    mem.clear();

    std::ifstream GBinary("06-ld-r-r.gb", std::ios::binary);
    if (!GBinary) {
        std::cerr << "Failed to open ROM\n";
        return 1;
    }
    GBinary.read(reinterpret_cast<char*>(mem.data), 0x8000);

    setPostBootRomState(cpu);
    std::ofstream logfile("debug.txt");

    std::cout << "starting execution" << std::endl;
    try {
        cpu.execute(5000000, mem, &logfile,false);
    }
    catch (const std::runtime_error& e) {
        std::cout << std::flush; 
        std::cerr << "\nError: " << e.what() << "\n"
                  << std::hex
                  << "A=" << int(cpu.A) << " F=" << int(cpu.getF())
                  << " B=" << int(cpu.B) << " C=" << int(cpu.C)
                  << " D=" << int(cpu.D) << " E=" << int(cpu.E)
                  << " H=" << int(cpu.H) << " L=" << int(cpu.L)
                  << " SP=" << int(cpu.SP) << "\n";
        return 1;
    }
    return 0;
}
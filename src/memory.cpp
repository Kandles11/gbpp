#include <memory.hpp>

void Memory::clear() {
    for (int i = 0x0000; i <= 0xFFFF; i++){
        data[i] = 0x00;
    }
}

uint8_t Memory::readMem(uint16_t addr) {
    if (addr == 0xFF44)
    {
        //lcd return
        return 0x90;
    }
    return data[addr];
}
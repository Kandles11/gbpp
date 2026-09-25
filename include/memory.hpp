#include <iostream>
#include <cstdint>

struct Memory {
    uint8_t data[0xFFFF];

    void clear();
    uint8_t readMem(uint16_t addr);
};


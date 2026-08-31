#include <memory.hpp>

void Memory::clear() {
    for (int i = 0x0000; i <= 0xFFFF; i++){
        data[i] = 0x00;
    }
}
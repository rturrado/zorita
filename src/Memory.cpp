#include "zorita/Memory.hpp"

namespace zorita {

Memory::Memory() { data_.fill(0xFFFF); }

uint16_t Memory::read(uint16_t address) { return data_[address]; }

void Memory::write(uint16_t address, uint16_t value) { data_[address] = value; }

} // namespace zorita

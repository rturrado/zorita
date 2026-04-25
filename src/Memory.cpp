#include "zorita/Memory.hpp"

#include "zorita/Error.hpp"

namespace zorita {

Memory::Memory() { data_.fill(0xFFFF); }

uint16_t Memory::read(uint16_t address) { return data_[address]; }

void Memory::write(uint16_t address, uint16_t value) { data_[address] = value; }

void Memory::write_block(uint16_t address, const uint16_t *block,
                         uint16_t size_in_words) {
  if (data_.begin() + address + size_in_words > data_.end()) {
    throw MemoryError{"insufficient memory space"};
  }
  std::copy(block, block + size_in_words, data_.begin() + address);
}

} // namespace zorita

#include "zorita/Registers.hpp"

#include <cassert>

namespace zorita {

Registers::Registers() = default;

uint16_t Registers::rx(uint8_t index) const {
  assert(index < NUM_DATA_REGISTERS);
  return rx_[index];
}

uint16_t Registers::ip() const { return ip_; }

StatusRegister &Registers::st() { return st_; }

void Registers::set_rx(uint8_t index, uint16_t value) {
  assert(index < NUM_DATA_REGISTERS);
  rx_[index] = value;
}

void Registers::set_ip(uint16_t value) { ip_ = value; }

void Registers::set_st(uint16_t value) { st_.set_st(value); }

} // namespace zorita

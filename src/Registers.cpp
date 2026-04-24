#include "zorita/Registers.hpp"

#include <cassert>
#include <cstddef>

namespace zorita {

// StatusRegister

StatusRegister::StatusRegister() : data_{0} {}

bool StatusRegister::zf() const { return data_[ZERO_FLAG_INDEX]; }

bool StatusRegister::nf() const { return data_[NEGATIVE_FLAG_INDEX]; }

bool StatusRegister::of() const { return data_[OVERFLOW_FLAG_INDEX]; }

void StatusRegister::set_st(uint16_t value) { data_ = value; }

void StatusRegister::set_flag(Flag flag, bool value) {
  size_t index = static_cast<std::size_t>(flag);
  assert(index <= LAST_FLAG_INDEX);
  data_[index] = value;
}

// Registers

Registers::Registers() : rx_{}, ip_{0}, st_{} {}

uint16_t Registers::rx(uint8_t index) const {
  assert(index < DATA_REGISTERS_SIZE);
  return rx_[index];
}

uint16_t Registers::ip() const { return ip_; }

StatusRegister &Registers::st() { return st_; }

void Registers::set_rx(uint8_t index, uint16_t value) {
  assert(index < DATA_REGISTERS_SIZE);
  rx_[index] = value;
}

void Registers::set_ip(uint16_t value) { ip_ = value; }

void Registers::set_st(uint16_t value) { st_.set_st(value); }

} // namespace zorita

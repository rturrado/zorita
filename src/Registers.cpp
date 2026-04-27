#include "zorita/Registers.hpp"

#include "zorita/Error.hpp"

#include <algorithm>
#include <cassert>
#include <fmt/format.h>

namespace zorita {

Registers::Registers() = default;

Registers::Registers(const std::vector<uint16_t> &rx) {
  if (rx.size() > rx_.size()) {
    throw RegistersError{fmt::format(
        "invalid rx: size of vector is bigger than {}", rx_.size())};
  }
  std::ranges::copy(rx, rx_.begin());
}

uint16_t Registers::rx(uint8_t index) const {
  assert(index < num_data_registers);
  return rx_[index];
}

uint16_t Registers::ip() const { return ip_; }

StatusRegister &Registers::st() { return st_; }

void Registers::set_rx(uint8_t index, uint16_t value) {
  assert(index < num_data_registers);
  rx_[index] = value;
}

void Registers::set_ip(uint16_t value) { ip_ = value; }

void Registers::set_st(uint16_t value) { st_.set_st(value); }

} // namespace zorita

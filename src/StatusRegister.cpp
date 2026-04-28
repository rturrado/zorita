#include "zorita/StatusRegister.hpp"

#include "zorita/Error.hpp"

#include <cstddef>

namespace zorita {

StatusRegister::StatusRegister() : data_{0} {}

StatusRegister::StatusRegister(uint16_t value) : data_{value} {}

uint16_t StatusRegister::to_uint16() const {
  return static_cast<uint16_t>(data_.to_ulong());
}

bool StatusRegister::zf() const { return data_[zero_flag_index]; }

bool StatusRegister::nf() const { return data_[negative_flag_index]; }

bool StatusRegister::cf() const { return data_[carry_flag_index]; }

bool StatusRegister::of() const { return data_[overflow_flag_index]; }

void StatusRegister::set_st(uint16_t value) { data_ = value; }

void StatusRegister::set_flag(Flag flag, bool value) {
  auto index = static_cast<std::size_t>(flag);
  if (index > last_flag_index) {
    throw StatusRegisterError{"invalid flag index"};
  }
  data_[index] = value;
}

} // namespace zorita

#include "zorita/StatusRegister.hpp"

#include "zorita/Error.hpp"

#include <cstddef>

namespace zorita {

StatusRegister::StatusRegister() : data_{0} {}

StatusRegister::StatusRegister(uint16_t value) : data_{value} {}

bool StatusRegister::zf() const { return data_[ZERO_FLAG_INDEX]; }

bool StatusRegister::nf() const { return data_[NEGATIVE_FLAG_INDEX]; }

bool StatusRegister::cf() const { return data_[CARRY_FLAG_INDEX]; }

bool StatusRegister::of() const { return data_[OVERFLOW_FLAG_INDEX]; }

void StatusRegister::set_st(uint16_t value) { data_ = value; }

void StatusRegister::set_flag(Flag flag, bool value) {
  size_t index = static_cast<std::size_t>(flag);
  if (index > LAST_FLAG_INDEX) {
    throw StatusRegisterError{"invalid flag index"};
  }
  data_[index] = value;
}

} // namespace zorita

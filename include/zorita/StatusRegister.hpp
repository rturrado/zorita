#pragma once

#include "zorita/MachineModel.hpp"

#include <bitset>
#include <cstdint>

namespace zorita {

/// @brief Status register flag identifiers.
enum class Flag : uint8_t {
  Zero = ZERO_FLAG_INDEX,
  Negative = NEGATIVE_FLAG_INDEX,
  Carry = CARRY_FLAG_INDEX,
  Overflow = OVERFLOW_FLAG_INDEX,
};

/// @brief The status register.
class StatusRegister {
public:
  /// @brief Initializes data_ to zero.
  StatusRegister();

  /// @brief Initializes data_ from value.
  StatusRegister(uint16_t value);

  /// @brief Returns true if the Zero flag is set.
  [[nodiscard]] bool zf() const;
  /// @brief Returns true if the Negative flag is set.
  [[nodiscard]] bool nf() const;
  /// @brief Returns true if the Carry flag is set.
  [[nodiscard]] bool cf() const;
  /// @brief Returns true if the Overflow flag is set.
  [[nodiscard]] bool of() const;

  /// @brief Returns an uint16_t representation of the data.
  [[nodiscard]] uint16_t to_uint16() const;

  /// @brief Sets the status register.
  /// @param value New status register value.
  void set_st(uint16_t value);

  /// @brief Sets a named flag with a given value.
  /// @param flag The flag to modify.
  /// @param value The value to set (default: true).
  void set_flag(Flag flag, bool value = true);

private:
  std::bitset<REGISTER_SIZE> data_;
};

} // namespace zorita

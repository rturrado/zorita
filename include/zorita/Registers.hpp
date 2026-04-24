#pragma once

#include "zorita/MachineModel.hpp"

#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>

namespace zorita {

/// @brief Status register flag identifiers.
enum class Flag : uint8_t {
  Zero = ZERO_FLAG_INDEX,
  Negative = NEGATIVE_FLAG_INDEX,
  Overflow = OVERFLOW_FLAG_INDEX,
};

/// @brief The status register.
class StatusRegister {
public:
  StatusRegister();

  /// @brief Returns true if the Zero flag is set.
  bool zf() const;
  /// @brief Returns true if the Negative flag is set.
  bool nf() const;
  /// @brief Returns true if the Overflow flag is set.
  bool of() const;

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

/// @brief The machine registers: data registers (R0–R7), instruction pointer
/// (IP), and status register (ST).
class Registers {
public:
  /// @brief Initializes all registers to zero.
  Registers();

  /// @brief Returns the value of data register Rx.
  /// @param index Register index in [0, DATA_REGISTERS_SIZE).
  uint16_t rx(uint8_t index) const;

  /// @brief Returns the current instruction pointer value.
  uint16_t ip() const;

  /// @brief Returns a reference to the status register.
  StatusRegister &st();

  /// @brief Sets a data register.
  /// @param index Data register index in [0, DATA_REGISTERS_SIZE).
  /// @param value New data register value.
  void set_rx(uint8_t index, uint16_t value);

  /// @brief Sets the instruction pointer.
  /// @param value New instruction pointer value.
  void set_ip(uint16_t value);

  /// @brief Sets the status register.
  /// @param value New status register value.
  void set_st(uint16_t value);

private:
  std::array<uint16_t, DATA_REGISTERS_SIZE> rx_;
  uint16_t ip_;
  StatusRegister st_;
};

} // namespace zorita

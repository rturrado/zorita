#pragma once

#include "zorita/MachineModel.hpp"
#include "zorita/StatusRegister.hpp"

#include <array>
#include <cstdint>
#include <vector>

namespace zorita {

using DataRegisters = std::array<uint16_t, num_data_registers>;

/// @brief The machine registers: data registers (R0 to R7), instruction pointer
/// (IP), and status register (ST).
class Registers {
public:
  /// @brief Initializes all registers to zero.
  Registers();

  /// @brief Initializes registers from user data.
  Registers(const std::vector<uint16_t> &rx);

  /// @brief Returns the value of data register Rx.
  /// @param index Register index in [0, DATA_REGISTERS_SIZE).
  [[nodiscard]] uint16_t rx(uint8_t index) const;

  /// @brief Returns the current instruction pointer value.
  [[nodiscard]] uint16_t ip() const;

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
  DataRegisters rx_{};
  uint16_t ip_{0};
  StatusRegister st_;
};

} // namespace zorita

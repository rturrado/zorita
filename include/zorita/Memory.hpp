#pragma once

#include "zorita/MachineModel.hpp"

#include <array>
#include <cstdint>

namespace zorita {

/// @brief Models the machine's memory.
class Memory {
public:
  /// @brief Initializes all memory positions to 0xFFFF.
  Memory();

  /// @brief Reads the 16-bit word at the given address.
  /// @param address Memory address to read from.
  /// @return The 16-bit value stored at that address.
  uint16_t read(uint16_t address);

  /// @brief Writes a 16-bit value to the given address.
  /// @param address Memory address to write to.
  /// @param value The 16-bit value to store.
  void write(uint16_t address, uint16_t value);

  /// @brief Writes a block of 16-bit values to the given address.
  /// @param address Memory address to write to.
  /// @param block The pointer to the block of 16-bit values.
  /// @param size_in_words The size of the block in words.
  void write_block(uint16_t address, const uint16_t *block,
                   uint16_t size_in_words);

private:
  std::array<uint16_t, MEMORY_SIZE> data_;
};

} // namespace zorita

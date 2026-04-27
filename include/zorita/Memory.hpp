#pragma once

#include "zorita/MachineModel.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace zorita {

/// @brief Models the machine's memory.
class Memory {
public:
  /// @brief Initializes all memory positions to 0xffff.
  Memory();

  /// @brief Initializes memory from data, filling remaining positions with
  /// 0xffff.
  /// @throws MemoryError if data is bigger than the memory size.
  Memory(const std::vector<uint16_t> &data);

  /// @brief Reads the 16-bit word at the given address.
  /// @param address Memory address to read from.
  /// @return The 16-bit value stored at that address.
  uint16_t read(uint16_t address);

  /// @brief Writes a 16-bit value to the given address.
  /// @param address Memory address to write to.
  /// @param value The 16-bit value to store.
  void write(uint16_t address, uint16_t value);

private:
  std::array<uint16_t, memory_size> data_{};
};

} // namespace zorita

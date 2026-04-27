#pragma once

#include "zorita/Condition.hpp"

#include <cstdint>
#include <variant>

namespace zorita {

class Machine;

/// @brief Instruction opcode.
enum class InstructionOpcode : uint8_t {
  Halt = 0,
  Cmp = 1,
  Jmp = 2,
  Load = 3,
  Store = 4,
  Add = 5,
  Sub = 6,
  Invalid = 7,
};

namespace instruction {

/// @brief Stops the machine.
struct Halt {
  bool operator==(const Halt &) const = default;
};

/// @brief Compares two data registers and updates the status register flags.
struct Cmp {
  uint8_t op1_;
  uint8_t op2_;
  bool operator==(const Cmp &) const = default;
};

/// @brief Jumps to an address held in a register, if a condition is met.
/// @note For an unconditional jump, Condition is encoded as an Always value.
struct Jmp {
  Condition condition_;
  uint8_t address_;
  bool operator==(const Jmp &) const = default;
};

/// @brief Loads a memory value into a register.
struct Load {
  uint8_t reg_;
  uint8_t addr_;
  bool operator==(const Load &) const = default;
};

/// @brief Stores a register value into memory.
struct Store {
  uint8_t reg_;
  uint8_t addr_;
  bool operator==(const Store &) const = default;
};

/// @brief Adds two registers and stores the result in a third register,
/// updating flags.
struct Add {
  uint8_t dst_;
  uint8_t src1_;
  uint8_t src2_;
  bool operator==(const Add &) const = default;
};

/// @brief Subtracts one register from another and stores the result in a third
/// register, updating flags.
struct Sub {
  uint8_t dst_;
  uint8_t src1_;
  uint8_t src2_;
  bool operator==(const Sub &) const = default;
};

} // namespace instruction

using namespace instruction;

using Instruction = std::variant<Halt, Cmp, Jmp, Load, Store, Add, Sub>;

} // namespace zorita

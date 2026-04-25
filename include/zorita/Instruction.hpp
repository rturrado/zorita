#pragma once

#include "zorita/Condition.hpp"
#include "zorita/MachineModel.hpp"

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
struct Halt {};

/// @brief Compares two data registers and updates the status register flags.
struct Cmp {
  uint8_t op1;
  uint8_t op2;
};

/// @brief Jumps to an address held in a register, if a condition is met.
/// @note For an unconditional jump, Condition is encoded as an Always value.
struct Jmp {
  Condition condition;
  uint8_t address;
};

/// @brief Loads a memory value into a register.
struct Load {
  uint8_t reg;
  uint8_t addr;
};

/// @brief Stores a register value into memory.
struct Store {
  uint8_t reg;
  uint8_t addr;
};

/// @brief Adds two registers and stores the result in a third register,
/// updating flags.
struct Add {
  uint8_t dst;
  uint8_t src1;
  uint8_t src2;
};

/// @brief Subtracts one register from another and stores the result in a third
/// register, updating flags.
struct Sub {
  uint8_t dst;
  uint8_t src1;
  uint8_t src2;
};

} // namespace instruction

using namespace instruction;

using Instruction = std::variant<Halt, Cmp, Jmp, Load, Store, Add, Sub>;

} // namespace zorita

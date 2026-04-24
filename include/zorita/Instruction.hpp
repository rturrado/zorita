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
class Halt {
public:
  Halt();

  /// @brief Sets the machine state to Stopped.
  void execute(Machine *machine);
};

/// @brief Compares two data registers and updates the status register flags.
class Cmp {
public:
  /// @param op1 Index of the first operand register.
  /// @param op2 Index of the second operand register.
  Cmp(uint8_t op1, uint8_t op2);

  void execute(Machine *machine);

private:
  uint8_t op1_;
  uint8_t op2_;
};

/// @brief Jumps to an address held in a register, if a condition is met.
/// @note For an unconditional jump, Condition is encoded as an Always value.
class Jmp {
public:
  /// @param condition Condition that must hold for the jump to occur.
  /// @param address Index of the register holding the address.
  Jmp(Condition condition, uint8_t address);

  void execute(Machine *machine);

private:
  Condition condition_;
  uint8_t address_;
};

/// @brief Loads a memory value into a register.
class Load {
public:
  /// @param reg Index of the destination register.
  /// @param addr Index of the register holding the memory address.
  Load(uint8_t reg, uint8_t addr);

  void execute(Machine *machine);

private:
  uint8_t reg_;
  uint8_t addr_;
};

/// @brief Stores a register value into memory.
class Store {
public:
  /// @param reg Index of the source register.
  /// @param addr Index of the register holding the memory address.
  Store(uint8_t reg, uint8_t addr);

  void execute(Machine *machine);

private:
  uint8_t reg_;
  uint8_t addr_;
};

/// @brief Adds two registers and stores the result in a third register,
/// updating flags.
class Add {
public:
  /// @param dst Index of the destination register.
  /// @param src1 Index of the first source register.
  /// @param src2 Index of the second source register.
  Add(uint8_t dst, uint8_t src1, uint8_t src2);

  void execute(Machine *machine);

private:
  uint8_t dst_;
  uint8_t src1_;
  uint8_t src2_;
};

/// @brief Subtracts one register from another and stores the result in a third
/// register, updating flags.
class Sub {
public:
  /// @param dst Index of the destination register.
  /// @param src1 Index of the minuend register.
  /// @param src2 Index of the subtrahend register.
  Sub(uint8_t dst, uint8_t src1, uint8_t src2);

  void execute(Machine *machine);

private:
  uint8_t dst_;
  uint8_t src1_;
  uint8_t src2_;
};

} // namespace instruction

using namespace instruction;

using Instruction = std::variant<Halt, Cmp, Jmp, Load, Store, Add, Sub>;

} // namespace zorita

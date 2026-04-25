#pragma once

#include "zorita/Decoder.hpp"
#include "zorita/Error.hpp"
#include "zorita/Instruction.hpp"
#include "zorita/Memory.hpp"
#include "zorita/Registers.hpp"

#include <filesystem>

namespace zorita {

/// @brief Execution state of the machine.
enum class State : bool {
  Stopped = 0,
  Running = 1,
};

/// @brief The RISC machine
class Machine {
public:
  Machine();

  /// @brief Loads a binary program image from disk into memory.
  /// @param program Path to the binary program file.
  /// @return Reference to this machine.
  /// @throws MachineError if the file cannot be opened or is too large.
  Machine &load(std::filesystem::path program);

  /// @brief Runs the machine until the program finishes or an error occurs.
  /// @throws MachineError if an error is encountered.
  void run();

  /// @brief Runs the machine until the program finishes or an error occurs.
  /// @throws MachineError if an error is encountered.
  void step();

private:
  /// @brief Sets the status register from the 32-bit result of an arithmetic operation.
  /// @param res32 32-bit result of an arithmetic operation (add or sub).
  void set_st_from(int32_t res32);

  /// @brief Executes a generic Instruction.
  /// @param instruction Generic Instruction.
  void execute(const Instruction &instruction);

  /// @brief Executes a Halt instruction.
  /// @param halt Halt instruction.
  void execute(const Halt &/* halt */);

  /// @brief Executes a Cmp instruction.
  /// @param cmp Cmp instruction.
  void execute(const Cmp &cmp);

  /// @brief Executes a Jmp instruction.
  /// @param jmp Jmp instruction.
  void execute(const Jmp &jmp);

  /// @brief Executes a Load instruction.
  /// @param load Load instruction.
  void execute(const Load &load);

  /// @brief Executes a Store instruction.
  /// @param store Store instruction.
  void execute(const Store &store);

  /// @brief Executes an Add instruction.
  /// @param add Add instruction.
  void execute(const Add &add);

  /// @brief Executes a Sub instruction.
  /// @param sub Sub instruction.
  void execute(const Sub &sub);

public:
  /// @brief Returns a reference to the machine's memory.
  Memory &memory();

  /// @brief Returns the value of data register Rx.
  /// @param index Register index in [0, DATA_REGISTERS_SIZE).
  uint16_t rx(uint8_t index) const;

  /// @brief Returns the current instruction pointer value.
  uint16_t ip() const;

  /// @brief Returns a reference to the status register.
  StatusRegister &st();

  /// @brief Returns the current execution state.
  State state() const;

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

  /// @brief Sets the execution state.
  /// @param state New execution state.
  void set_state(State state);

private:
  Decoder decoder_;
  Memory memory_;
  Registers registers_;
  State state_;
};

} // namespace zorita

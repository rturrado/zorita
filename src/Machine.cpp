#include "zorita/Machine.hpp"

#include "zorita/Decoder.hpp"
#include "zorita/Error.hpp"

#include <filesystem>
#include <fstream>

namespace zorita {

Machine::Machine() = default;

void Machine::load(const std::filesystem::path &program) {
  // Check program is a valid file path
  if (!std::filesystem::is_regular_file(program)) {
    throw MachineError{"invalid program path"};
  }
  memory_.load(program);
}

void Machine::run() {
  // Read the address of the first instruction from memory position 0
  // Then set IP
  registers_.set_ip(memory_.read(0));
  // Set machine state to Running
  state_ = State::Running;
  // Loop until machine state is Stopped or an error occurs
  while (state_ != State::Stopped) {
    step();
  }
}

void Machine::step() {
  // Read current instruction from memory
  uint16_t word = memory_.read(registers_.ip());
  // Update IP to point to the next instruction
  registers_.set_ip(registers_.ip() + 1);
  Instruction instruction = Decoder::decode(word);
  execute(instruction);
}

void Machine::set_st_from(int32_t res32) {
  int16_t res16 = static_cast<int16_t>(res32);
  st().set_flag(Flag::Zero, res16 == 0);
  st().set_flag(Flag::Negative, res16 < 0);
  // Carry flag is set directly at Cmp, Add, and Sub
  st().set_flag(Flag::Overflow, res32 != res16);
}

void Machine::execute(const Instruction &instruction) {
  std::visit([this](auto &&inst) { this->execute(inst); }, instruction);
}

void Machine::execute(const Halt &) { set_state(State::Stopped); }

void Machine::execute(const Cmp &cmp) {
  uint16_t uval1 = rx(cmp.op1);
  uint16_t uval2 = rx(cmp.op2);
  // Calculate 32-bit result
  int32_t val1 = static_cast<int16_t>(uval1);
  int32_t val2 = static_cast<int16_t>(uval2);
  int32_t res32 = val1 - val2;
  // Update status register
  set_st_from(res32);
  st().set_flag(Flag::Carry, uval1 < uval2);
}

void Machine::execute(const Jmp &jmp) {
  if (jmp.condition.check(st())) {
    set_ip(rx(jmp.address));
  }
}

void Machine::execute(const Load &load) {
  uint16_t address = rx(load.addr);
  uint16_t value = memory().read(address);
  set_rx(load.reg, value);
}

void Machine::execute(const Store &store) {
  uint16_t address = rx(store.addr);
  uint16_t value = rx(store.reg);
  memory().write(address, value);
}

void Machine::execute(const Add &add) {
  uint32_t uval1 = rx(add.src1);
  uint32_t uval2 = rx(add.src2);
  // Calculate 32-bit result
  int32_t val1 = static_cast<int16_t>(uval1);
  int32_t val2 = static_cast<int16_t>(uval2);
  int32_t res32 = val1 + val2;
  // Get 16-bit result out of 32-bit result
  int16_t res16 = static_cast<int16_t>(res32);
  // Update registers
  set_rx(add.dst, res16);
  set_st_from(res32);
  st().set_flag(Flag::Carry, uval1 + uval2 > 0xffff);
}

void Machine::execute(const Sub &sub) {
  uint16_t uval1 = rx(sub.src1);
  uint16_t uval2 = rx(sub.src2);
  // Calculate 32-bit result
  int32_t val1 = static_cast<int16_t>(uval1);
  int32_t val2 = static_cast<int16_t>(uval2);
  int32_t res32 = val1 - val2;
  // Get 16-bit result out of 32-bit result
  int16_t res16 = static_cast<int16_t>(res32);
  // Update registers
  set_rx(sub.dst, res16);
  set_st_from(res32);
  st().set_flag(Flag::Carry, uval1 < uval2);
}

Memory &Machine::memory() { return memory_; }

StatusRegister &Machine::st() { return registers_.st(); }

uint16_t Machine::rx(uint8_t index) const { return registers_.rx(index); }

uint16_t Machine::ip() const { return registers_.ip(); }

State Machine::state() const { return state_; }

void Machine::set_ip(uint16_t value) { registers_.set_ip(value); }

void Machine::set_rx(uint8_t index, uint16_t value) {
  registers_.set_rx(index, value);
}

void Machine::set_st(uint16_t value) { registers_.set_st(value); }

void Machine::set_state(State state) { state_ = state; }

} // namespace zorita

#include "zorita/Instruction.hpp"

#include "zorita/Machine.hpp"

namespace zorita::instruction {

void update_flags(Machine *machine, int32_t res32) {
  int16_t res16 = static_cast<int16_t>(res32);
  machine->st().set_flag(Flag::Zero, res16 == 0);
  machine->st().set_flag(Flag::Negative, res16 < 0);
  machine->st().set_flag(Flag::Overflow, res32 != res16);
}

Halt::Halt() {}

void Halt::execute(Machine *machine) { machine->set_state(State::Stopped); }

Cmp::Cmp(uint8_t op1, uint8_t op2) : op1_{op1}, op2_{op2} {}

void Cmp::execute(Machine *machine) {
  int16_t val1 = static_cast<int16_t>(machine->rx(op1_));
  int16_t val2 = static_cast<int16_t>(machine->rx(op2_));
  int32_t res32 = static_cast<int32_t>(val1) - static_cast<int32_t>(val2);
  update_flags(machine, res32);
}

Jmp::Jmp(Condition condition, uint8_t address)
    : condition_{condition}, address_{address} {}

void Jmp::execute(Machine *machine) {
  if (condition_.check(machine->st())) {
    machine->set_ip(machine->rx(address_));
  }
}

Load::Load(uint8_t reg, uint8_t addr) : reg_{reg}, addr_{addr} {}

void Load::execute(Machine *machine) {
  uint16_t address = machine->rx(addr_);
  uint16_t value = machine->memory().read(address);
  machine->set_rx(reg_, value);
}

Store::Store(uint8_t reg, uint8_t addr) : reg_{reg}, addr_{addr} {}

void Store::execute(Machine *machine) {
  uint16_t address = machine->rx(addr_);
  uint16_t value = machine->rx(reg_);
  machine->memory().write(address, value);
}

Add::Add(uint8_t dst, uint8_t src1, uint8_t src2)
    : dst_{dst}, src1_{src1}, src2_{src2} {}

void Add::execute(Machine *machine) {
  int16_t val1 = static_cast<int16_t>(machine->rx(src1_));
  int16_t val2 = static_cast<int16_t>(machine->rx(src2_));
  int32_t res32 = static_cast<int32_t>(val1) + static_cast<int32_t>(val2);
  int16_t res16 = static_cast<int16_t>(res32);
  machine->set_rx(dst_, res16);
  update_flags(machine, res32);
}

Sub::Sub(uint8_t dst, uint8_t src1, uint8_t src2)
    : dst_{dst}, src1_{src1}, src2_{src2} {}

void Sub::execute(Machine *machine) {
  int16_t val1 = static_cast<int16_t>(machine->rx(src1_));
  int16_t val2 = static_cast<int16_t>(machine->rx(src2_));
  int32_t res32 = static_cast<int32_t>(val1) - static_cast<int32_t>(val2);
  int16_t res16 = static_cast<int16_t>(res32);
  machine->set_rx(dst_, res16);
  update_flags(machine, res32);
}

} // namespace zorita::instruction

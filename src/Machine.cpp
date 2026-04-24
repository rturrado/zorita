#include "zorita/Machine.hpp"

namespace zorita {

Machine::Machine() : state_{State::Stopped} {}

Machine &Machine::load(std::filesystem::path /*program*/) { return *this; }

void Machine::run() {
  registers_.set_ip(memory_.read(0));
  state_ = State::Running;
  while (state_ == State::Running) {
    uint16_t word = memory_.read(registers_.ip());
    registers_.set_ip(registers_.ip() + 1);
    Instruction instruction = decoder_.decode(word);
    std::visit([this](auto &&inst) { inst.execute(this); }, instruction);
  }
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

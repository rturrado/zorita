#include "zorita/Decoder.hpp"

#include "zorita/Error.hpp"

namespace zorita {

uint8_t Decoder::instruction_opcode(uint16_t word) {
  return word >> instruction_opcode_begin;
}

uint8_t Decoder::cmp_op1_opcode(uint16_t word) {
  return (word >> cmp_op1_opcode_begin) & register_opcode_mask;
}

uint8_t Decoder::cmp_op2_opcode(uint16_t word) {
  return (word >> cmp_op2_opcode_begin) & register_opcode_mask;
}

uint8_t Decoder::jmp_cond_opcode(uint16_t word) {
  return (word >> jmp_cond_opcode_begin) & condition_opcode_mask;
}

uint8_t Decoder::jmp_addr_opcode(uint16_t word) {
  return (word >> jmp_addr_opcode_begin) & register_opcode_mask;
}

uint8_t Decoder::load_reg_opcode(uint16_t word) {
  return (word >> load_reg_opcode_begin) & register_opcode_mask;
}

uint8_t Decoder::load_addr_opcode(uint16_t word) {
  return (word >> load_addr_opcode_begin) & register_opcode_mask;
}

uint8_t Decoder::store_reg_opcode(uint16_t word) {
  return (word >> store_reg_opcode_begin) & register_opcode_mask;
}

uint8_t Decoder::store_addr_opcode(uint16_t word) {
  return (word >> store_addr_opcode_begin) & register_opcode_mask;
}

uint8_t Decoder::add_dst_opcode(uint16_t word) {
  return (word >> add_dst_opcode_begin) & register_opcode_mask;
}

uint8_t Decoder::add_src1_opcode(uint16_t word) {
  return (word >> add_src1_opcode_begin) & register_opcode_mask;
}

uint8_t Decoder::add_src2_opcode(uint16_t word) {
  return (word >> add_src2_opcode_begin) & register_opcode_mask;
}

uint8_t Decoder::sub_dst_opcode(uint16_t word) {
  return (word >> sub_dst_opcode_begin) & register_opcode_mask;
}

uint8_t Decoder::sub_src1_opcode(uint16_t word) {
  return (word >> sub_src1_opcode_begin) & register_opcode_mask;
}

uint8_t Decoder::sub_src2_opcode(uint16_t word) {
  return (word >> sub_src2_opcode_begin) & register_opcode_mask;
}

Instruction Decoder::decode(uint16_t word) {
  auto opcode = InstructionOpcode{instruction_opcode(word)};
  switch (opcode) {
  case InstructionOpcode::Halt:
    return Halt{};
  case InstructionOpcode::Cmp: {
    auto op1 = cmp_op1_opcode(word);
    auto op2 = cmp_op2_opcode(word);
    return Cmp{.op1_ = op1, .op2_ = op2};
  }
  case InstructionOpcode::Jmp: {
    auto cond = ConditionOpcode{jmp_cond_opcode(word)};
    auto addr = jmp_addr_opcode(word);
    return Jmp{.condition_ = Condition(cond), .address_ = addr};
  }
  case InstructionOpcode::Load: {
    auto reg = load_reg_opcode(word);
    auto addr = load_addr_opcode(word);
    return Load{.reg_ = reg, .addr_ = addr};
  }
  case InstructionOpcode::Store: {
    auto reg = store_reg_opcode(word);
    auto addr = store_addr_opcode(word);
    return Store{.reg_ = reg, .addr_ = addr};
  }
  case InstructionOpcode::Add: {
    auto dst = add_dst_opcode(word);
    auto src1 = add_src1_opcode(word);
    auto src2 = add_src2_opcode(word);
    return Add{.dst_ = dst, .src1_ = src1, .src2_ = src2};
  }
  case InstructionOpcode::Sub: {
    auto dst = sub_dst_opcode(word);
    auto src1 = sub_src1_opcode(word);
    auto src2 = sub_src2_opcode(word);
    return Sub{.dst_ = dst, .src1_ = src1, .src2_ = src2};
  }
  case InstructionOpcode::Invalid:
  default:
    throw DecoderError{"invalid instruction"};
  }
}

} // namespace zorita

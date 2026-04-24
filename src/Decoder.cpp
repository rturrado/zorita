#include "zorita/Decoder.hpp"

#include "zorita/Error.hpp"

namespace zorita {

uint8_t Decoder::instruction_opcode(uint16_t word) {
  return word >> INSTRUCTION_OPCODE_BEGIN;
}

uint8_t Decoder::cmp_op1_opcode(uint16_t word) {
  return (word >> CMP_OP1_OPCODE_BEGIN) & DATA_REGISTER_OPCODE_MASK;
}

uint8_t Decoder::cmp_op2_opcode(uint16_t word) {
  return (word >> CMP_OP2_OPCODE_BEGIN) & DATA_REGISTER_OPCODE_MASK;
}

uint8_t Decoder::jmp_cond_opcode(uint16_t word) {
  return (word >> JMP_COND_OPCODE_BEGIN) & CONDITION_OPCODE_MASK;
}

uint8_t Decoder::jmp_addr_opcode(uint16_t word) {
  return (word >> JMP_ADDR_OPCODE_BEGIN) & DATA_REGISTER_OPCODE_MASK;
}

uint8_t Decoder::load_reg_opcode(uint16_t word) {
  return (word >> LOAD_REG_OPCODE_BEGIN) & DATA_REGISTER_OPCODE_MASK;
}

uint8_t Decoder::load_addr_opcode(uint16_t word) {
  return (word >> LOAD_ADDR_OPCODE_BEGIN) & DATA_REGISTER_OPCODE_MASK;
}

uint8_t Decoder::store_reg_opcode(uint16_t word) {
  return (word >> STORE_REG_OPCODE_BEGIN) & DATA_REGISTER_OPCODE_MASK;
}

uint8_t Decoder::store_addr_opcode(uint16_t word) {
  return (word >> STORE_ADDR_OPCODE_BEGIN) & DATA_REGISTER_OPCODE_MASK;
}

uint8_t Decoder::add_dst_opcode(uint16_t word) {
  return (word >> ADD_DST_OPCODE_BEGIN) & DATA_REGISTER_OPCODE_MASK;
}

uint8_t Decoder::add_src1_opcode(uint16_t word) {
  return (word >> ADD_SRC1_OPCODE_BEGIN) & DATA_REGISTER_OPCODE_MASK;
}

uint8_t Decoder::add_src2_opcode(uint16_t word) {
  return (word >> ADD_SRC2_OPCODE_BEGIN) & DATA_REGISTER_OPCODE_MASK;
}

uint8_t Decoder::sub_dst_opcode(uint16_t word) {
  return (word >> SUB_DST_OPCODE_BEGIN) & DATA_REGISTER_OPCODE_MASK;
}

uint8_t Decoder::sub_src1_opcode(uint16_t word) {
  return (word >> SUB_SRC1_OPCODE_BEGIN) & DATA_REGISTER_OPCODE_MASK;
}

uint8_t Decoder::sub_src2_opcode(uint16_t word) {
  return (word >> SUB_SRC2_OPCODE_BEGIN) & DATA_REGISTER_OPCODE_MASK;
}

Instruction Decoder::decode(uint16_t word) {
  auto opcode = InstructionOpcode{instruction_opcode(word)};
  switch (opcode) {
  case InstructionOpcode::Halt:
    return Halt{};
  case InstructionOpcode::Cmp: {
    auto op1 = cmp_op1_opcode(word);
    auto op2 = cmp_op2_opcode(word);
    return Cmp{op1, op2};
  }
  case InstructionOpcode::Jmp: {
    auto cond = ConditionOpcode{jmp_cond_opcode(word)};
    auto addr = jmp_addr_opcode(word);
    return Jmp{Condition(cond), addr};
  }
  case InstructionOpcode::Load: {
    auto reg = load_reg_opcode(word);
    auto addr = load_addr_opcode(word);
    return Load{reg, addr};
  }
  case InstructionOpcode::Store: {
    auto reg = store_reg_opcode(word);
    auto addr = store_addr_opcode(word);
    return Store{reg, addr};
  }
  case InstructionOpcode::Add: {
    auto dst = add_dst_opcode(word);
    auto src1 = add_src1_opcode(word);
    auto src2 = add_src2_opcode(word);
    return Add{dst, src1, src2};
  }
  case InstructionOpcode::Sub: {
    auto dst = sub_dst_opcode(word);
    auto src1 = sub_src1_opcode(word);
    auto src2 = sub_src2_opcode(word);
    return Sub{dst, src1, src2};
  }
  case InstructionOpcode::Invalid:
  default:
    throw DecoderError{"invalid instruction"};
  }
}

} // namespace zorita

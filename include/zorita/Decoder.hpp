#pragma once

#include "zorita/Instruction.hpp"

#include <cstdint>

namespace zorita {

/// @brief Decodes a raw 16-bit memory word into an Instruction variant.
class Decoder {
public:
  /// @brief Decodes a raw 16-bit memory word into an Instruction variant.
  /// @param word Raw value read from memory.
  /// @return The decoded instruction.
  /// @throws DecoderError if the opcode is invalid.
  static Instruction decode(uint16_t word);

private:
  static uint8_t instruction_opcode(uint16_t word);
  static uint8_t cmp_op1_opcode(uint16_t word);
  static uint8_t cmp_op2_opcode(uint16_t word);
  static uint8_t jmp_cond_opcode(uint16_t word);
  static uint8_t jmp_addr_opcode(uint16_t word);
  static uint8_t load_reg_opcode(uint16_t word);
  static uint8_t load_addr_opcode(uint16_t word);
  static uint8_t store_reg_opcode(uint16_t word);
  static uint8_t store_addr_opcode(uint16_t word);
  static uint8_t add_dst_opcode(uint16_t word);
  static uint8_t add_src1_opcode(uint16_t word);
  static uint8_t add_src2_opcode(uint16_t word);
  static uint8_t sub_dst_opcode(uint16_t word);
  static uint8_t sub_src1_opcode(uint16_t word);
  static uint8_t sub_src2_opcode(uint16_t word);
};

} // namespace zorita

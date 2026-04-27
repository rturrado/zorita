#pragma once

#include <cstdint>

namespace zorita {

/// @brief Size of memory in 16-bit words.
constexpr uint32_t memory_size = 65536;

/// @brief Size of a register in bits.
constexpr uint8_t register_size = 16;

/// @brief Number of data registers (R0 to R7).
constexpr uint8_t num_data_registers = 8;

/// @name Status register flag indices.
/// @{
constexpr uint8_t zero_flag_index = 0;
constexpr uint8_t negative_flag_index = 1;
constexpr uint8_t carry_flag_index = 2;
constexpr uint8_t overflow_flag_index = 3;
constexpr uint8_t last_flag_index = 3;
/// @}

/// @name Instruction encoding
/// @{
constexpr uint8_t instruction_opcode_begin = 13;
constexpr uint8_t cmp_op1_opcode_begin = 10;
constexpr uint8_t cmp_op2_opcode_begin = 7;
constexpr uint8_t jmp_cond_opcode_begin = 9;
constexpr uint8_t jmp_addr_opcode_begin = 6;
constexpr uint8_t load_reg_opcode_begin = 10;
constexpr uint8_t load_addr_opcode_begin = 7;
constexpr uint8_t store_reg_opcode_begin = 10;
constexpr uint8_t store_addr_opcode_begin = 7;
constexpr uint8_t add_dst_opcode_begin = 10;
constexpr uint8_t add_src1_opcode_begin = 7;
constexpr uint8_t add_src2_opcode_begin = 4;
constexpr uint8_t sub_dst_opcode_begin = 10;
constexpr uint8_t sub_src1_opcode_begin = 7;
constexpr uint8_t sub_src2_opcode_begin = 4;
constexpr uint8_t register_opcode_mask = 0b111;
constexpr uint8_t condition_opcode_mask = 0b1111;
/// @}

/// @name Condition encoding
/// @{
constexpr uint8_t condition_reserved_opcode_1 = 6;
constexpr uint8_t condition_reserved_opcode_2 = 14;
constexpr uint8_t condition_last_valid_opcode = 15;
/// @}

} // namespace zorita

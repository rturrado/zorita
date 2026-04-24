#pragma once

#include <array>
#include <cstdint>

namespace zorita {

/// @brief Size of memory in 16-bit words.
constexpr uint32_t MEMORY_SIZE = 65536;

/// @brief Width of a register in bits.
constexpr uint8_t REGISTER_SIZE = 16;

/// @brief Number of data registers (R0–R7).
constexpr uint8_t DATA_REGISTERS_SIZE = 8;

/// @name Status register flag indices.
/// @{
constexpr uint8_t ZERO_FLAG_INDEX = 0;
constexpr uint8_t NEGATIVE_FLAG_INDEX = 1;
constexpr uint8_t OVERFLOW_FLAG_INDEX = 2;
constexpr uint8_t LAST_FLAG_INDEX = 2;
/// @}

/// @name Instruction encoding
/// @{
constexpr uint8_t INSTRUCTION_OPCODE_BEGIN = 13;
constexpr uint8_t CMP_OP1_OPCODE_BEGIN = 10;
constexpr uint8_t CMP_OP2_OPCODE_BEGIN = 7;
constexpr uint8_t JMP_COND_OPCODE_BEGIN = 9;
constexpr uint8_t JMP_ADDR_OPCODE_BEGIN = 6;
constexpr uint8_t LOAD_REG_OPCODE_BEGIN = 10;
constexpr uint8_t LOAD_ADDR_OPCODE_BEGIN = 7;
constexpr uint8_t STORE_REG_OPCODE_BEGIN = 10;
constexpr uint8_t STORE_ADDR_OPCODE_BEGIN = 7;
constexpr uint8_t ADD_DST_OPCODE_BEGIN = 10;
constexpr uint8_t ADD_SRC1_OPCODE_BEGIN = 7;
constexpr uint8_t ADD_SRC2_OPCODE_BEGIN = 4;
constexpr uint8_t SUB_DST_OPCODE_BEGIN = 10;
constexpr uint8_t SUB_SRC1_OPCODE_BEGIN = 7;
constexpr uint8_t SUB_SRC2_OPCODE_BEGIN = 4;
constexpr uint8_t DATA_REGISTER_OPCODE_MASK = 0b111;
constexpr uint8_t CONDITION_OPCODE_MASK = 0b1111;
/// @}

} // namespace zorita

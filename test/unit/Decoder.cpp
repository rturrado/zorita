#include "zorita/Decoder.hpp"
#include "zorita/Error.hpp"
#include "zorita/Instruction.hpp"
#include "zorita/MachineModel.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace zorita {

using namespace instruction;

class DecoderTest : public ::testing::Test {
protected:
  void SetUp() override {}
  uint16_t halt_opcode{0x0 << INSTRUCTION_OPCODE_BEGIN};
  Halt halt_instruction{};
  uint16_t cmp_opcode{0x1 << INSTRUCTION_OPCODE_BEGIN |
                      0x1 << CMP_OP1_OPCODE_BEGIN |
                      0x2 << CMP_OP2_OPCODE_BEGIN};
  Cmp cmp_instruction{0x1, 0x2};
  uint16_t jmp_opcode{0x2 << INSTRUCTION_OPCODE_BEGIN |
                      0x7 << JMP_COND_OPCODE_BEGIN |
                      0x1 << JMP_ADDR_OPCODE_BEGIN};
  Jmp jmp_instruction{ConditionOpcode{0x7}, 0x1};
  uint16_t load_opcode{0x3 << INSTRUCTION_OPCODE_BEGIN |
                       0x1 << LOAD_REG_OPCODE_BEGIN |
                       0x2 << LOAD_ADDR_OPCODE_BEGIN};
  Load load_instruction{0x1, 0x2};
  uint16_t store_opcode{0x4 << INSTRUCTION_OPCODE_BEGIN |
                        0x1 << STORE_REG_OPCODE_BEGIN |
                        0x2 << STORE_ADDR_OPCODE_BEGIN};
  Store store_instruction{0x1, 0x2};
  uint16_t add_opcode{
      0x5 << INSTRUCTION_OPCODE_BEGIN | 0x1 << ADD_DST_OPCODE_BEGIN |
      0x2 << ADD_SRC1_OPCODE_BEGIN | 0x3 << ADD_SRC2_OPCODE_BEGIN};
  Add add_instruction{0x1, 0x2, 0x3};
  uint16_t sub_opcode{
      0x6 << INSTRUCTION_OPCODE_BEGIN | 0x1 << SUB_DST_OPCODE_BEGIN |
      0x2 << SUB_SRC1_OPCODE_BEGIN | 0x3 << SUB_SRC2_OPCODE_BEGIN};
  Sub sub_instruction{0x1, 0x2, 0x3};
  uint16_t invalid_opcode{0x7 << INSTRUCTION_OPCODE_BEGIN};
  Decoder decoder;
};

TEST_F(DecoderTest, decode) {
  auto instruction = decoder.decode(halt_opcode);
  EXPECT_TRUE(std::holds_alternative<Halt>(instruction));
  EXPECT_EQ(std::get<Halt>(instruction), halt_instruction);

  instruction = decoder.decode(cmp_opcode);
  EXPECT_TRUE(std::holds_alternative<Cmp>(instruction));
  EXPECT_EQ(std::get<Cmp>(instruction), cmp_instruction);

  instruction = decoder.decode(jmp_opcode);
  EXPECT_TRUE(std::holds_alternative<Jmp>(instruction));
  EXPECT_EQ(std::get<Jmp>(instruction), jmp_instruction);

  instruction = decoder.decode(load_opcode);
  EXPECT_TRUE(std::holds_alternative<Load>(instruction));
  EXPECT_EQ(std::get<Load>(instruction), load_instruction);

  instruction = decoder.decode(store_opcode);
  EXPECT_TRUE(std::holds_alternative<Store>(instruction));
  EXPECT_EQ(std::get<Store>(instruction), store_instruction);

  instruction = decoder.decode(add_opcode);
  EXPECT_TRUE(std::holds_alternative<Add>(instruction));
  EXPECT_EQ(std::get<Add>(instruction), add_instruction);

  instruction = decoder.decode(sub_opcode);
  EXPECT_TRUE(std::holds_alternative<Sub>(instruction));
  EXPECT_EQ(std::get<Sub>(instruction), sub_instruction);

  EXPECT_THAT([&]() { decoder.decode(invalid_opcode); },
              testing::ThrowsMessage<DecoderError>("invalid instruction"));
}

} // namespace zorita
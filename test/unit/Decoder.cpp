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
  uint16_t halt_opcode_{0x0 << instruction_opcode_begin};
  Halt halt_instruction_{};
  uint16_t cmp_opcode_{0x1 << instruction_opcode_begin |
                       0x1 << cmp_op1_opcode_begin |
                       0x2 << cmp_op2_opcode_begin};
  Cmp cmp_instruction_{.op1_ = 0x1, .op2_ = 0x2};
  uint16_t jmp_opcode_{0x2 << instruction_opcode_begin |
                       0x7 << jmp_cond_opcode_begin |
                       0x1 << jmp_addr_opcode_begin};
  Jmp jmp_instruction_{.condition_ = ConditionOpcode{0x7}, .address_ = 0x1};
  uint16_t load_opcode_{0x3 << instruction_opcode_begin |
                        0x1 << load_reg_opcode_begin |
                        0x2 << load_addr_opcode_begin};
  Load load_instruction_{.reg_ = 0x1, .addr_ = 0x2};
  uint16_t store_opcode_{0x4 << instruction_opcode_begin |
                         0x1 << store_reg_opcode_begin |
                         0x2 << store_addr_opcode_begin};
  Store store_instruction_{.reg_ = 0x1, .addr_ = 0x2};
  uint16_t add_opcode_{
      0x5 << instruction_opcode_begin | 0x1 << add_dst_opcode_begin |
      0x2 << add_src1_opcode_begin | 0x3 << add_src2_opcode_begin};
  Add add_instruction_{.dst_ = 0x1, .src1_ = 0x2, .src2_ = 0x3};
  uint16_t sub_opcode_{
      0x6 << instruction_opcode_begin | 0x1 << sub_dst_opcode_begin |
      0x2 << sub_src1_opcode_begin | 0x3 << sub_src2_opcode_begin};
  Sub sub_instruction_{.dst_ = 0x1, .src1_ = 0x2, .src2_ = 0x3};
  uint16_t invalid_opcode_{0x7 << instruction_opcode_begin};
};

TEST_F(DecoderTest, decode) {
  auto instruction = Decoder::decode(halt_opcode_);
  EXPECT_TRUE(std::holds_alternative<Halt>(instruction));
  EXPECT_EQ(std::get<Halt>(instruction), halt_instruction_);

  instruction = Decoder::decode(cmp_opcode_);
  EXPECT_TRUE(std::holds_alternative<Cmp>(instruction));
  EXPECT_EQ(std::get<Cmp>(instruction), cmp_instruction_);

  instruction = Decoder::decode(jmp_opcode_);
  EXPECT_TRUE(std::holds_alternative<Jmp>(instruction));
  EXPECT_EQ(std::get<Jmp>(instruction), jmp_instruction_);

  instruction = Decoder::decode(load_opcode_);
  EXPECT_TRUE(std::holds_alternative<Load>(instruction));
  EXPECT_EQ(std::get<Load>(instruction), load_instruction_);

  instruction = Decoder::decode(store_opcode_);
  EXPECT_TRUE(std::holds_alternative<Store>(instruction));
  EXPECT_EQ(std::get<Store>(instruction), store_instruction_);

  instruction = Decoder::decode(add_opcode_);
  EXPECT_TRUE(std::holds_alternative<Add>(instruction));
  EXPECT_EQ(std::get<Add>(instruction), add_instruction_);

  instruction = Decoder::decode(sub_opcode_);
  EXPECT_TRUE(std::holds_alternative<Sub>(instruction));
  EXPECT_EQ(std::get<Sub>(instruction), sub_instruction_);

  EXPECT_THAT([&]() { Decoder::decode(invalid_opcode_); },
              testing::ThrowsMessage<DecoderError>("invalid instruction"));
}

} // namespace zorita
#include "zorita/Condition.hpp"
#include "zorita/Error.hpp"
#include "zorita/StatusRegister.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace zorita {

class ConditionTest : public ::testing::Test {
protected:
  void SetUp() override {}
  //  3  2  1  0  index within the status register
  // OF CF NF ZF  flag
  StatusRegister st0x0_{0x00}; // ZF=0 NF=0 CF=0 OF=0
  StatusRegister st0x1_{0x01}; // ZF=1
  StatusRegister st0x2_{0x02}; // NF=1
  StatusRegister st0x4_{0x04}; // CF=1
  StatusRegister st0x8_{0x08}; // OF=1
  StatusRegister st0xa_{0x0a}; // NF=1 OF=1
  StatusRegister st0xb_{0x0b}; // ZF=1 NF=1 OF=1
  Condition zero_{ConditionOpcode::Zero};
  Condition negative_{ConditionOpcode::Negative};
  Condition carry_{ConditionOpcode::Carry};
  Condition overflow_{ConditionOpcode::Overflow};
  Condition less_than_{ConditionOpcode::LessThan};
  Condition less_or_equal_{ConditionOpcode::LessOrEqual};
  Condition reserved_1_{ConditionOpcode{condition_reserved_opcode_1}};
  Condition always_{ConditionOpcode::Always};
  Condition not_zero_{ConditionOpcode::NotZero};
  Condition not_negative_{ConditionOpcode::NotNegative};
  Condition not_carry_{ConditionOpcode::NotCarry};
  Condition not_overflow_{ConditionOpcode::NotOverflow};
  Condition greater_or_equal_{ConditionOpcode::GreaterOrEqual};
  Condition greater_than_{ConditionOpcode::GreaterThan};
  Condition reserved_2_{ConditionOpcode{condition_reserved_opcode_2}};
  Condition never_{ConditionOpcode::Never};
  Condition invalid_{ConditionOpcode{condition_last_valid_opcode + 1}};
};

TEST_F(ConditionTest, check) {
  EXPECT_EQ(zero_.check(st0x0_), false);
  EXPECT_EQ(zero_.check(st0x1_), true);

  EXPECT_EQ(negative_.check(st0x0_), false);
  EXPECT_EQ(negative_.check(st0x2_), true);

  EXPECT_EQ(carry_.check(st0x0_), false);
  EXPECT_EQ(carry_.check(st0x4_), true);

  EXPECT_EQ(overflow_.check(st0x0_), false);
  EXPECT_EQ(overflow_.check(st0x8_), true);

  EXPECT_EQ(less_than_.check(st0x0_), false); // NF=OF=0
  EXPECT_EQ(less_than_.check(st0x2_), true);  // NF=1, OF=0
  EXPECT_EQ(less_than_.check(st0x8_), true);  // NF=0, OF=1
  EXPECT_EQ(less_than_.check(st0xa_), false); // NF=OF=1

  EXPECT_EQ(less_or_equal_.check(st0x0_), false); // NF=OF=0, ZF=0
  EXPECT_EQ(less_or_equal_.check(st0x1_), true);  // ZF=1
  EXPECT_EQ(less_or_equal_.check(st0x2_), true);  // NF=1, OF=0
  EXPECT_EQ(less_or_equal_.check(st0x8_), true);  // NF=0, OF=1
  EXPECT_EQ(less_or_equal_.check(st0xa_), false); // NF=OF=1, ZF=0
  EXPECT_EQ(less_or_equal_.check(st0xb_), true);  // NF=OF=1, ZF=1

  EXPECT_EQ(always_.check(st0x0_), true);

  EXPECT_EQ(not_zero_.check(st0x0_), true);
  EXPECT_EQ(not_zero_.check(st0x1_), false);

  EXPECT_EQ(not_negative_.check(st0x0_), true);
  EXPECT_EQ(not_negative_.check(st0x2_), false);

  EXPECT_EQ(not_carry_.check(st0x0_), true);
  EXPECT_EQ(not_carry_.check(st0x4_), false);

  EXPECT_EQ(not_overflow_.check(st0x0_), true);
  EXPECT_EQ(not_overflow_.check(st0x8_), false);

  EXPECT_EQ(greater_or_equal_.check(st0x0_), true);  // NF=OF=0
  EXPECT_EQ(greater_or_equal_.check(st0xa_), true);  // NF=OF=1
  EXPECT_EQ(greater_or_equal_.check(st0x2_), false); // NF=1, OF=0
  EXPECT_EQ(greater_or_equal_.check(st0x8_), false); // NF=0, OF=1

  EXPECT_EQ(greater_than_.check(st0x0_), true);  // NF=OF=0, ZF=0
  EXPECT_EQ(greater_than_.check(st0xa_), true);  // NF=OF=1, ZF=0
  EXPECT_EQ(greater_than_.check(st0x1_), false); // ZF=1
  EXPECT_EQ(greater_than_.check(st0xb_), false); // NF=OF=1, ZF=1
  EXPECT_EQ(greater_than_.check(st0x2_), false); // NF=1, OF=0
  EXPECT_EQ(greater_than_.check(st0x8_), false); // NF=0, OF=1

  EXPECT_EQ(never_.check(st0x0_), false);

  EXPECT_THAT([&]() { (void)reserved_1_.check(st0x0_); },
              testing::ThrowsMessage<ConditionError>("invalid condition code"));
  EXPECT_THAT([&]() { (void)reserved_2_.check(st0x0_); },
              testing::ThrowsMessage<ConditionError>("invalid condition code"));
  EXPECT_THAT([&]() { (void)invalid_.check(st0x0_); },
              testing::ThrowsMessage<ConditionError>("invalid condition code"));
}

} // namespace zorita

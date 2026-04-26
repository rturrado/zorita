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
  StatusRegister st0x0{0x00}; // ZF=0 NF=0 CF=0 OF=0
  StatusRegister st0x1{0x01}; // ZF=1
  StatusRegister st0x2{0x02}; // NF=1
  StatusRegister st0x4{0x04}; // CF=1
  StatusRegister st0x8{0x08}; // OF=1
  StatusRegister st0xa{0x0a}; // NF=1 OF=1
  StatusRegister st0xb{0x0b}; // ZF=1 NF=1 OF=1
  Condition zero{ConditionOpcode::Zero};
  Condition negative{ConditionOpcode::Negative};
  Condition carry{ConditionOpcode::Carry};
  Condition overflow{ConditionOpcode::Overflow};
  Condition less_than{ConditionOpcode::LessThan};
  Condition less_or_equal{ConditionOpcode::LessOrEqual};
  Condition reserved_1{ConditionOpcode{CONDITION_RESERVED_OPCODE_1}};
  Condition always{ConditionOpcode::Always};
  Condition not_zero{ConditionOpcode::NotZero};
  Condition not_negative{ConditionOpcode::NotNegative};
  Condition not_carry{ConditionOpcode::NotCarry};
  Condition not_overflow{ConditionOpcode::NotOverflow};
  Condition greater_or_equal{ConditionOpcode::GreaterOrEqual};
  Condition greater_than{ConditionOpcode::GreaterThan};
  Condition reserved_2{ConditionOpcode{CONDITION_RESERVED_OPCODE_2}};
  Condition never{ConditionOpcode::Never};
  Condition invalid{ConditionOpcode{CONDITION_LAST_VALID_OPCODE + 1}};
};

TEST_F(ConditionTest, check) {
  EXPECT_EQ(zero.check(st0x0), false);
  EXPECT_EQ(zero.check(st0x1), true);

  EXPECT_EQ(negative.check(st0x0), false);
  EXPECT_EQ(negative.check(st0x2), true);

  EXPECT_EQ(carry.check(st0x0), false);
  EXPECT_EQ(carry.check(st0x4), true);

  EXPECT_EQ(overflow.check(st0x0), false);
  EXPECT_EQ(overflow.check(st0x8), true);

  EXPECT_EQ(less_than.check(st0x0), false); // NF=OF=0
  EXPECT_EQ(less_than.check(st0x2), true);  // NF=1, OF=0
  EXPECT_EQ(less_than.check(st0x8), true);  // NF=0, OF=1
  EXPECT_EQ(less_than.check(st0xa), false); // NF=OF=1

  EXPECT_EQ(less_or_equal.check(st0x0), false); // NF=OF=0, ZF=0
  EXPECT_EQ(less_or_equal.check(st0x1), true);  // ZF=1
  EXPECT_EQ(less_or_equal.check(st0x2), true);  // NF=1, OF=0
  EXPECT_EQ(less_or_equal.check(st0x8), true);  // NF=0, OF=1
  EXPECT_EQ(less_or_equal.check(st0xa), false); // NF=OF=1, ZF=0
  EXPECT_EQ(less_or_equal.check(st0xb), true);  // NF=OF=1, ZF=1

  EXPECT_EQ(always.check(st0x0), true);

  EXPECT_EQ(not_zero.check(st0x0), true);
  EXPECT_EQ(not_zero.check(st0x1), false);

  EXPECT_EQ(not_negative.check(st0x0), true);
  EXPECT_EQ(not_negative.check(st0x2), false);

  EXPECT_EQ(not_carry.check(st0x0), true);
  EXPECT_EQ(not_carry.check(st0x4), false);

  EXPECT_EQ(not_overflow.check(st0x0), true);
  EXPECT_EQ(not_overflow.check(st0x8), false);

  EXPECT_EQ(greater_or_equal.check(st0x0), true);  // NF=OF=0
  EXPECT_EQ(greater_or_equal.check(st0xa), true);  // NF=OF=1
  EXPECT_EQ(greater_or_equal.check(st0x2), false); // NF=1, OF=0
  EXPECT_EQ(greater_or_equal.check(st0x8), false); // NF=0, OF=1

  EXPECT_EQ(greater_than.check(st0x0), true);  // NF=OF=0, ZF=0
  EXPECT_EQ(greater_than.check(st0xa), true);  // NF=OF=1, ZF=0
  EXPECT_EQ(greater_than.check(st0x1), false); // ZF=1
  EXPECT_EQ(greater_than.check(st0xb), false); // NF=OF=1, ZF=1
  EXPECT_EQ(greater_than.check(st0x2), false); // NF=1, OF=0
  EXPECT_EQ(greater_than.check(st0x8), false); // NF=0, OF=1

  EXPECT_EQ(never.check(st0x0), false);

  EXPECT_THAT([&]() { (void)reserved_1.check(st0x0); },
              testing::ThrowsMessage<ConditionError>("invalid condition code"));
  EXPECT_THAT([&]() { (void)reserved_2.check(st0x0); },
              testing::ThrowsMessage<ConditionError>("invalid condition code"));
  EXPECT_THAT([&]() { (void)invalid.check(st0x0); },
              testing::ThrowsMessage<ConditionError>("invalid condition code"));
}

} // namespace zorita

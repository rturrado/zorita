#include "zorita/StatusRegister.hpp"
#include "zorita/Error.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace zorita {

class StatusRegisterTest : public ::testing::Test {
protected:
  //  3  2  1  0  index within the status register
  // OF CF NF ZF  flag
  StatusRegister st0x0_{0x00}; // OF=0 CF=0 NF=0 ZF=0
  StatusRegister st0x1_{0x01}; //                ZF=1
  StatusRegister st0x2_{0x02}; //           NF=1
  StatusRegister st0x4_{0x04}; //      CF=1
  StatusRegister st0x6_{0x06}; //      CF=1 NF=1
  StatusRegister st0x8_{0x08}; // OF=1
  StatusRegister st0xa_{0x0a}; // OF=1      NF=1
  StatusRegister st_;
};

TEST_F(StatusRegisterTest, StatusRegister) {
  EXPECT_FALSE(st_.zf());
  EXPECT_FALSE(st_.nf());
  EXPECT_FALSE(st_.cf());
  EXPECT_FALSE(st_.of());
}

TEST_F(StatusRegisterTest, zf) {
  EXPECT_FALSE(st0x0_.zf());
  EXPECT_TRUE(st0x1_.zf());
}

TEST_F(StatusRegisterTest, nf) {
  EXPECT_FALSE(st0x0_.nf());
  EXPECT_TRUE(st0x2_.nf());
}

TEST_F(StatusRegisterTest, cf) {
  EXPECT_FALSE(st0x0_.cf());
  EXPECT_TRUE(st0x4_.cf());
}

TEST_F(StatusRegisterTest, of) {
  EXPECT_FALSE(st0x0_.of());
  EXPECT_TRUE(st0x8_.of());
}

TEST_F(StatusRegisterTest, to_uint16_t) {
  EXPECT_EQ(st0x6_.to_uint16(), 0x06);
  EXPECT_EQ(st0xa_.to_uint16(), 0x0a);
}

TEST_F(StatusRegisterTest, set_st) {
  st_.set_st(0x06);
  EXPECT_FALSE(st_.zf());
  EXPECT_TRUE(st_.nf());
  EXPECT_TRUE(st_.cf());
  EXPECT_FALSE(st_.of());
}

TEST_F(StatusRegisterTest, set_flag) {
  st_.set_flag(Flag::Zero, true);
  EXPECT_TRUE(st_.zf());
  EXPECT_THAT(
      // clang-format off
      [&]() { st_.set_flag(Flag{0xff}, true); }, // NOLINT(clang-analyzer-optin.core.EnumCastOutOfRange)
      // clang-format on
      testing::ThrowsMessage<StatusRegisterError>("invalid flag index"));
}

} // namespace zorita

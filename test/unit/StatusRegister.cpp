#include "zorita/StatusRegister.hpp"
#include "zorita/Error.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace zorita {

class StatusRegisterTest : public ::testing::Test {
protected:
  //  3  2  1  0  index within the status register
  // OF CF NF ZF  flag
  StatusRegister st0x0{0x00}; // ZF=0 NF=0 CF=0 OF=0
  StatusRegister st0x1{0x01}; // ZF=1
  StatusRegister st0x2{0x02}; // NF=1
  StatusRegister st0x4{0x04}; // CF=1
  StatusRegister st0x8{0x08}; // OF=1
  StatusRegister st{};
};

TEST_F(StatusRegisterTest, StatusRegister) {
  EXPECT_FALSE(st.zf());
  EXPECT_FALSE(st.nf());
  EXPECT_FALSE(st.cf());
  EXPECT_FALSE(st.of());
}

TEST_F(StatusRegisterTest, zf) {
  EXPECT_FALSE(st0x0.zf());
  EXPECT_TRUE(st0x1.zf());
}

TEST_F(StatusRegisterTest, nf) {
  EXPECT_FALSE(st0x0.nf());
  EXPECT_TRUE(st0x2.nf());
}

TEST_F(StatusRegisterTest, cf) {
  EXPECT_FALSE(st0x0.cf());
  EXPECT_TRUE(st0x4.cf());
}

TEST_F(StatusRegisterTest, of) {
  EXPECT_FALSE(st0x0.of());
  EXPECT_TRUE(st0x8.of());
}

TEST_F(StatusRegisterTest, set_st) {
  st.set_st(0x06);
  EXPECT_FALSE(st.zf());
  EXPECT_TRUE(st.nf());
  EXPECT_TRUE(st.cf());
  EXPECT_FALSE(st.of());
}

TEST_F(StatusRegisterTest, set_flag) {
  st.set_flag(Flag::Zero, true);
  EXPECT_TRUE(st.zf());
  EXPECT_THAT(
      [&]() { st.set_flag(Flag{0xff}, true); },
      testing::ThrowsMessage<StatusRegisterError>("invalid flag index"));
}

} // namespace zorita

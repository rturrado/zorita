#include "zorita/Registers.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace zorita {

class RegistersTest : public ::testing::Test {
protected:
  Registers registers;
};

TEST_F(RegistersTest, Registers) {
  for (uint8_t i = 0; i < NUM_DATA_REGISTERS; ++i) {
    EXPECT_EQ(registers.rx(i), 0);
  }
  EXPECT_EQ(registers.ip(), 0);
  EXPECT_FALSE(registers.st().zf());
  EXPECT_FALSE(registers.st().nf());
  EXPECT_FALSE(registers.st().cf());
  EXPECT_FALSE(registers.st().of());
}

TEST_F(RegistersTest, rx) {
  registers.set_rx(0, 0x0000);
  EXPECT_EQ(registers.rx(0), 0x0000);
  registers.set_rx(7, 0xffff);
  EXPECT_EQ(registers.rx(7), 0xffff);
}

TEST_F(RegistersTest, ip) {
  registers.set_ip(0x8000);
  EXPECT_EQ(registers.ip(), 0x8000);
}

TEST_F(RegistersTest, set_st) {
  registers.set_st(0x05); // ZF=1, CF=1
  auto &st = registers.st();
  EXPECT_TRUE(st.zf());
  EXPECT_FALSE(st.nf());
  EXPECT_TRUE(st.cf());
  EXPECT_FALSE(st.of());
}

} // namespace zorita

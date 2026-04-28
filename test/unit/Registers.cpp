#include "zorita/Registers.hpp"
#include "zorita/Error.hpp"

#include <fmt/format.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace zorita {

class RegistersTest : public ::testing::Test {
protected:
  Registers registers_;
};

TEST_F(RegistersTest, default_construtor) {
  for (uint8_t i = 0; i < num_data_registers; ++i) {
    EXPECT_EQ(registers_.rx(i), 0);
  }
  EXPECT_EQ(registers_.ip(), 0);
  EXPECT_EQ(registers_.st().to_uint16(), 0);
}

TEST(Registers, construct_from_valid_data) {
  std::vector<uint16_t> rx{0, 1, 2, 3};
  Registers registers{rx};
  for (uint8_t i = 0; i < rx.size(); ++i) {
    EXPECT_EQ(registers.rx(i), rx[i]);
  }
  for (uint8_t i = rx.size(); i < num_data_registers; ++i) {
    EXPECT_EQ(registers.rx(i), 0);
  }
}

TEST(Registers, construct_from_invalid_data) {
  std::vector<uint16_t> invalid_rx(num_data_registers + 1, 0x0000);
  EXPECT_THAT(
      [&]() { Registers{invalid_rx}; },
      testing::ThrowsMessage<RegistersError>(fmt::format(
          "invalid rx: size of vector is bigger than {}", num_data_registers)));
}

TEST_F(RegistersTest, rx) {
  registers_.set_rx(0, 0x0000);
  EXPECT_EQ(registers_.rx(0), 0x0000);
  registers_.set_rx(7, 0xffff);
  EXPECT_EQ(registers_.rx(7), 0xffff);
}

TEST_F(RegistersTest, ip) {
  registers_.set_ip(0x8000);
  EXPECT_EQ(registers_.ip(), 0x8000);
}

TEST_F(RegistersTest, set_st) {
  registers_.set_st(0x05); // OF=0, CF=1, NF=0, ZF=1
  auto &st = registers_.st();
  EXPECT_EQ(st.to_uint16(), 0x05);
}

} // namespace zorita

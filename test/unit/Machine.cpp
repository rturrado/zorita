#include "zorita/Machine.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace zorita {

class MachineTest : public ::testing::Test {
protected:
  Machine machine;
  std::vector<uint16_t> halt_program{
      0x0001, // ip = 1
      0x0000, // halt
  };
  std::vector<uint16_t> cmp_program{
      0x0001, // ip = 1
      0x2080, // cmp r0, r1
  };
  std::vector<uint16_t> jmp_zero_program{
      0x0001, // ip = 1
      0x4000, // jmp r0 if ZF=1
  };
  std::vector<uint16_t> jmp_negative_program{
      0x0001, // ip = 1
      0x4200, // jmp r0 if NF=1
  };
  std::vector<uint16_t> jmp_carry_program{
      0x0001, // ip = 1
      0x4400, // jmp r0 if CF=1
  };
  std::vector<uint16_t> jmp_overflow_program{
      0x0001, // ip = 1
      0x4600, // jmp r0 if OF=1
  };
  std::vector<uint16_t> jmp_less_than_program{
      0x0001, // ip = 1
      0x4800, // jmp r0 if NF != OF
  };
  std::vector<uint16_t> jmp_less_or_equal_program{
      0x0001, // ip = 1
      0x4a00, // jmp r0 if ZF=1 or NF != OF
  };
  std::vector<uint16_t> jmp_always_program{
      0x0001, // ip = 1
      0x4e00, // jmp r0 always
  };
  std::vector<uint16_t> jmp_not_zero_program{
      0x0001, // ip = 1
      0x5000, // jmp r0 if ZF=0
  };
  std::vector<uint16_t> jmp_not_negative_program{
      0x0001, // ip = 1
      0x5200, // jmp r0 if NF=0
  };
  std::vector<uint16_t> jmp_not_carry_program{
      0x0001, // ip = 1
      0x5400, // jmp r0 if CF=0
  };
  std::vector<uint16_t> jmp_not_overflow_program{
      0x0001, // ip = 1
      0x5600, // jmp r0 if OF=0
  };
  std::vector<uint16_t> jmp_greater_or_equal_program{
      0x0001, // ip = 1
      0x5800, // jmp r0 if NF == OF
  };
  std::vector<uint16_t> jmp_greater_than_program{
      0x0001, // ip = 1
      0x5a00, // jmp r0 if ZF=0 and NF == OF
  };
  std::vector<uint16_t> jmp_never_program{
      0x0001, // ip = 1
      0x5e00, // jmp r0 never
  };
  std::vector<uint16_t> load_program{
      0x0002, // ip = 2
      0x1234, // address to be read from
      0x6080, // load r0, r1
  };
  std::vector<uint16_t> store_program{
      0x0002, // ip = 2
      0x0000, // address to be written to
      0x8080, // store r0, r1
  };
  std::vector<uint16_t> add_program{
      0x0001, // ip = 1
      0xa010, // add r0, r0, r1
  };
  std::vector<uint16_t> sub_program{
      0x0001, // ip = 1
      0xc010, // sub r0, r0, r1
  };
  Machine machine_halt{halt_program};
  Machine machine_cmp{cmp_program};
  Machine machine_jmp_zero{jmp_zero_program};
  Machine machine_jmp_negative{jmp_negative_program};
  Machine machine_jmp_carry{jmp_carry_program};
  Machine machine_jmp_overflow{jmp_overflow_program};
  Machine machine_jmp_less_than{jmp_less_than_program};
  Machine machine_jmp_less_or_equal{jmp_less_or_equal_program};
  Machine machine_jmp_always{jmp_always_program};
  Machine machine_jmp_not_zero{jmp_not_zero_program};
  Machine machine_jmp_not_negative{jmp_not_negative_program};
  Machine machine_jmp_not_carry{jmp_not_carry_program};
  Machine machine_jmp_not_overflow{jmp_not_overflow_program};
  Machine machine_jmp_greater_or_equal{jmp_greater_or_equal_program};
  Machine machine_jmp_greater_than{jmp_greater_than_program};
  Machine machine_jmp_never{jmp_never_program};
  Machine machine_load{load_program};
  Machine machine_store{store_program};
  Machine machine_add{add_program};
  Machine machine_sub{sub_program};
};

TEST_F(MachineTest, default_constructor) {
  EXPECT_EQ(machine.state(), State::Stopped);
}

TEST_F(MachineTest, run) {
  machine_halt.set_ip(0x0001);
  machine_halt.run();
  EXPECT_EQ(machine_halt.state(), State::Stopped);
}

TEST_F(MachineTest, step_halt) {
  machine_halt.set_ip(0x0001);
  machine_halt.step();
  EXPECT_EQ(machine_halt.state(), State::Stopped);
}

TEST_F(MachineTest, step_cmp) {
  // r0 == r1
  // OF=0, CF=0, NF=0, ZF=1
  machine_cmp.set_rx(0, 0x0010);
  machine_cmp.set_rx(1, 0x0010);
  machine_cmp.set_ip(0x0001);
  machine_cmp.step();
  EXPECT_EQ(machine_cmp.st().to_uint16(), 0x0001);

  // r0 > r1
  // OF=0, CF=0, NF=0, ZF=0
  machine_cmp.set_rx(0, 0x0010);
  machine_cmp.set_rx(1, 0x0001);
  machine_cmp.set_ip(0x0001);
  machine_cmp.step();
  EXPECT_EQ(machine_cmp.st().to_uint16(), 0x0000);

  // r0 < r1
  // NF is set because signed r0 - r1 is negative
  // CF is set because unsigned r0 - r1 wraps around
  // OF=0, CF=1, NF=1, ZF=0
  machine_cmp.set_rx(0, 0x0001);
  machine_cmp.set_rx(1, 0x0010);
  machine_cmp.set_ip(0x0001);
  machine_cmp.step();
  EXPECT_EQ(machine_cmp.st().to_uint16(), 0x0006);

  // Signed overflow: negative minus positive wraps to positive
  // OF=1, CF=0, NF=0, ZF=0
  machine_cmp.set_rx(0, 0x8000);
  machine_cmp.set_rx(1, 0x7fff);
  machine_cmp.set_ip(0x0001);
  machine_cmp.step();
  EXPECT_EQ(machine_cmp.st().to_uint16(), 0x0008);

  // Signed overflow: positive minus negative wraps to negative
  // OF=1, CF=1, NF=1, ZF=0
  machine_cmp.set_rx(0, 0x7fff);
  machine_cmp.set_rx(1, 0x8000);
  machine_cmp.set_ip(0x0001);
  machine_cmp.step();
  EXPECT_EQ(machine_cmp.st().to_uint16(), 0x000e);
}

TEST_F(MachineTest, step_jmp) {
  // jmp r0 if zero
  // r0=0x1000, ip=1
  machine_jmp_zero.set_rx(0, 0x1000);
  machine_jmp_zero.set_ip(0x0001);
  machine_jmp_zero.set_st(0x0000);
  machine_jmp_zero.step();
  EXPECT_EQ(machine_jmp_zero.ip(), 0x0002); // ZF=0, no jump
  machine_jmp_zero.set_ip(0x0001);
  machine_jmp_zero.set_st(0x0001);
  machine_jmp_zero.step();
  EXPECT_EQ(machine_jmp_zero.ip(), 0x1000); // ZF=1, jump

  // jmp r0 if negative
  // r0=0x1000, ip=1
  machine_jmp_negative.set_rx(0, 0x1000);
  machine_jmp_negative.set_ip(0x0001);
  machine_jmp_negative.set_st(0x0000);
  machine_jmp_negative.step();
  EXPECT_EQ(machine_jmp_negative.ip(), 0x0002); // NF=0, no jump
  machine_jmp_negative.set_ip(0x0001);
  machine_jmp_negative.set_st(0x0002);
  machine_jmp_negative.step();
  EXPECT_EQ(machine_jmp_negative.ip(), 0x1000); // NF=1, jump

  // jmp r0 if carry
  // r0=0x1000, ip=1
  machine_jmp_carry.set_rx(0, 0x1000);
  machine_jmp_carry.set_ip(0x0001);
  machine_jmp_carry.set_st(0x0000);
  machine_jmp_carry.step();
  EXPECT_EQ(machine_jmp_carry.ip(), 0x0002); // CF=0, no jump
  machine_jmp_carry.set_ip(0x0001);
  machine_jmp_carry.set_st(0x0004);
  machine_jmp_carry.step();
  EXPECT_EQ(machine_jmp_carry.ip(), 0x1000); // CF=1, jump

  // jmp r0 if overflow
  // r0=0x1000, ip=1
  machine_jmp_overflow.set_rx(0, 0x1000);
  machine_jmp_overflow.set_ip(0x0001);
  machine_jmp_overflow.set_st(0x0000);
  machine_jmp_overflow.step();
  EXPECT_EQ(machine_jmp_overflow.ip(), 0x0002); // OF=0, no jump
  machine_jmp_overflow.set_ip(0x0001);
  machine_jmp_overflow.set_st(0x0008);
  machine_jmp_overflow.step();
  EXPECT_EQ(machine_jmp_overflow.ip(), 0x1000); // OF=1, jump

  // jmp r0 if less than
  // r0=0x1000, ip=1
  machine_jmp_less_than.set_rx(0, 0x1000);
  machine_jmp_less_than.set_ip(0x0001);
  machine_jmp_less_than.set_st(0x0000);
  machine_jmp_less_than.step();
  EXPECT_EQ(machine_jmp_less_than.ip(), 0x0002); // NF=OF=0, no jump
  machine_jmp_less_than.set_ip(0x0001);
  machine_jmp_less_than.set_st(0x0002);
  machine_jmp_less_than.step();
  EXPECT_EQ(machine_jmp_less_than.ip(), 0x1000); // NF=1, OF=0, jump

  // jmp r0 if less or equal
  // r0=0x1000, ip=1
  machine_jmp_less_or_equal.set_rx(0, 0x1000);
  machine_jmp_less_or_equal.set_ip(0x0001);
  machine_jmp_less_or_equal.set_st(0x0000);
  machine_jmp_less_or_equal.step();
  EXPECT_EQ(machine_jmp_less_or_equal.ip(), 0x0002); // ZF=0, NF=OF=0, no jump
  machine_jmp_less_or_equal.set_ip(0x0001);
  machine_jmp_less_or_equal.set_st(0x0001);
  machine_jmp_less_or_equal.step();
  EXPECT_EQ(machine_jmp_less_or_equal.ip(), 0x1000); // ZF=1, jump

  // jmp r0 always
  // r0=0x1000, ip=1
  machine_jmp_always.set_rx(0, 0x1000);
  machine_jmp_always.set_ip(0x0001);
  machine_jmp_always.set_st(0x0000);
  machine_jmp_always.step();
  EXPECT_EQ(machine_jmp_always.ip(), 0x1000); // always jump

  // jmp r0 if not zero
  // r0=0x1000, ip=1
  machine_jmp_not_zero.set_rx(0, 0x1000);
  machine_jmp_not_zero.set_ip(0x0001);
  machine_jmp_not_zero.set_st(0x0001);
  machine_jmp_not_zero.step();
  EXPECT_EQ(machine_jmp_not_zero.ip(), 0x0002); // ZF=1, no jump
  machine_jmp_not_zero.set_ip(0x0001);
  machine_jmp_not_zero.set_st(0x0000);
  machine_jmp_not_zero.step();
  EXPECT_EQ(machine_jmp_not_zero.ip(), 0x1000); // ZF=0, jump

  // jmp r0 if not negative
  // r0=0x1000, ip=1
  machine_jmp_not_negative.set_rx(0, 0x1000);
  machine_jmp_not_negative.set_ip(0x0001);
  machine_jmp_not_negative.set_st(0x0002);
  machine_jmp_not_negative.step();
  EXPECT_EQ(machine_jmp_not_negative.ip(), 0x0002); // NF=1, no jump
  machine_jmp_not_negative.set_ip(0x0001);
  machine_jmp_not_negative.set_st(0x0000);
  machine_jmp_not_negative.step();
  EXPECT_EQ(machine_jmp_not_negative.ip(), 0x1000); // NF=0, jump

  // jmp r0 if not carry
  // r0=0x1000, ip=1
  machine_jmp_not_carry.set_rx(0, 0x1000);
  machine_jmp_not_carry.set_ip(0x0001);
  machine_jmp_not_carry.set_st(0x0004);
  machine_jmp_not_carry.step();
  EXPECT_EQ(machine_jmp_not_carry.ip(), 0x0002); // CF=1, no jump
  machine_jmp_not_carry.set_ip(0x0001);
  machine_jmp_not_carry.set_st(0x0000);
  machine_jmp_not_carry.step();
  EXPECT_EQ(machine_jmp_not_carry.ip(), 0x1000); // CF=0, jump

  // jmp r0 if not overflow
  // r0=0x1000, ip=1
  machine_jmp_not_overflow.set_rx(0, 0x1000);
  machine_jmp_not_overflow.set_ip(0x0001);
  machine_jmp_not_overflow.set_st(0x0008);
  machine_jmp_not_overflow.step();
  EXPECT_EQ(machine_jmp_not_overflow.ip(), 0x0002); // OF=1, no jump
  machine_jmp_not_overflow.set_ip(0x0001);
  machine_jmp_not_overflow.set_st(0x0000);
  machine_jmp_not_overflow.step();
  EXPECT_EQ(machine_jmp_not_overflow.ip(), 0x1000); // OF=0, jump

  // jmp r0 if greater or equal
  // r0=0x1000, ip=1
  machine_jmp_greater_or_equal.set_rx(0, 0x1000);
  machine_jmp_greater_or_equal.set_ip(0x0001);
  machine_jmp_greater_or_equal.set_st(0x0002);
  machine_jmp_greater_or_equal.step();
  EXPECT_EQ(machine_jmp_greater_or_equal.ip(), 0x0002); // NF=1, OF=0, no jump
  machine_jmp_greater_or_equal.set_ip(0x0001);
  machine_jmp_greater_or_equal.set_st(0x0000);
  machine_jmp_greater_or_equal.step();
  EXPECT_EQ(machine_jmp_greater_or_equal.ip(), 0x1000); // NF=OF=0, jump

  // jmp r0 if greater than
  // r0=0x1000, ip=1
  machine_jmp_greater_than.set_rx(0, 0x1000);
  machine_jmp_greater_than.set_ip(0x0001);
  machine_jmp_greater_than.set_st(0x0001);
  machine_jmp_greater_than.step();
  EXPECT_EQ(machine_jmp_greater_than.ip(), 0x0002); // ZF=1, no jump
  machine_jmp_greater_than.set_ip(0x0001);
  machine_jmp_greater_than.set_st(0x0000);
  machine_jmp_greater_than.step();
  EXPECT_EQ(machine_jmp_greater_than.ip(), 0x1000); // ZF=0, NF=OF=0, jump

  // jmp r0 never
  // r0=0x1000, ip=1
  machine_jmp_never.set_rx(0, 0x1000);
  machine_jmp_never.set_ip(0x0001);
  machine_jmp_never.set_st(0x0000);
  machine_jmp_never.step();
  EXPECT_EQ(machine_jmp_never.ip(), 0x0002); // never jump
}

TEST_F(MachineTest, step_load) {
  // load r0, r1
  // r1=1, memory[1]=0x1234
  machine_load.set_ip(0x0002);
  machine_load.set_rx(1, 0x0001);
  machine_load.step();
  EXPECT_EQ(machine_load.rx(0), 0x1234);
}

TEST_F(MachineTest, step_store) {
  // store r0, r1
  // r0=0x1234, r1=1
  machine_store.set_ip(0x0002);
  machine_store.set_rx(0, 0x1234);
  machine_store.set_rx(1, 0x0001);
  machine_store.step();
  EXPECT_EQ(machine_store.memory().read(1), 0x1234);
}

TEST_F(MachineTest, step_add) {
  // r0 + r1 == 0
  // OF=0, CF=0, NF=0, ZF=1
  machine_add.set_rx(0, 0x0000);
  machine_add.set_rx(1, 0x0000);
  machine_add.set_ip(0x0001);
  machine_add.step();
  EXPECT_EQ(machine_add.rx(0), 0x0000);
  EXPECT_EQ(machine_add.st().to_uint16(), 0x0001);

  // r0 + r1 > 0
  // OF=0, CF=0, NF=0, ZF=0
  machine_add.set_rx(0, 0x0001);
  machine_add.set_rx(1, 0x0001);
  machine_add.set_ip(0x0001);
  machine_add.step();
  EXPECT_EQ(machine_add.rx(0), 0x0002);
  EXPECT_EQ(machine_add.st().to_uint16(), 0x0000);

  // r0 + r1 < 0
  // OF=0, CF=0, NF=1, ZF=0
  machine_add.set_rx(0, 0x0001);
  machine_add.set_rx(1, 0xfffe);
  machine_add.set_ip(0x0001);
  machine_add.step();
  EXPECT_EQ(machine_add.rx(0), 0xffff);
  EXPECT_EQ(machine_add.st().to_uint16(), 0x0002);

  // CF is set because unsigned r0 + r1 wraps around
  // OF=0, CF=1, NF=0, ZF=0
  machine_add.set_rx(0, 0xff00);
  machine_add.set_rx(1, 0x0101);
  machine_add.set_ip(0x0001);
  machine_add.step();
  EXPECT_EQ(machine_add.rx(0), 0x0001);
  EXPECT_EQ(machine_add.st().to_uint16(), 0x0004);

  // Signed overflow: positive plus positive wraps to negative
  // OF=1, CF=0, NF=1, ZF=0
  machine_add.set_rx(0, 0x7fff);
  machine_add.set_rx(1, 0x0001);
  machine_add.set_ip(0x0001);
  machine_add.step();
  EXPECT_EQ(machine_add.rx(0), 0x8000);
  EXPECT_EQ(machine_add.st().to_uint16(), 0x000a);
}

TEST_F(MachineTest, step_sub) {
  // r0 == r1
  // OF=0, CF=0, NF=0, ZF=1
  machine_sub.set_rx(0, 0x0010);
  machine_sub.set_rx(1, 0x0010);
  machine_sub.set_ip(0x0001);
  machine_sub.step();
  EXPECT_EQ(machine_sub.rx(0), 0x0000);
  EXPECT_EQ(machine_sub.st().to_uint16(), 0x0001);

  // r0 > r1
  // OF=0, CF=0, NF=0, ZF=0
  machine_sub.set_rx(0, 0x0010);
  machine_sub.set_rx(1, 0x0001);
  machine_sub.set_ip(0x0001);
  machine_sub.step();
  EXPECT_EQ(machine_sub.rx(0), 0x000f);
  EXPECT_EQ(machine_sub.st().to_uint16(), 0x0000);

  // r0 < r1
  // NF is set because signed r0 - r1 is negative
  // CF is set because unsigned r0 - r1 wraps around
  // OF=0, CF=1, NF=1, ZF=0
  machine_sub.set_rx(0, 0x0001);
  machine_sub.set_rx(1, 0x0010);
  machine_sub.set_ip(0x0001);
  machine_sub.step();
  EXPECT_EQ(machine_sub.rx(0), 0xfff1);
  EXPECT_EQ(machine_sub.st().to_uint16(), 0x0006);

  // Signed overflow: negative minus positive wraps to positive
  // OF=1, CF=0, NF=0, ZF=0
  machine_sub.set_rx(0, 0x8000);
  machine_sub.set_rx(1, 0x7fff);
  machine_sub.set_ip(0x0001);
  machine_sub.step();
  EXPECT_EQ(machine_sub.rx(0), 0x0001);
  EXPECT_EQ(machine_sub.st().to_uint16(), 0x0008);

  // Signed overflow: positive minus negative wraps to negative
  // OF=1, CF=1, NF=1, ZF=0
  machine_sub.set_rx(0, 0x7fff);
  machine_sub.set_rx(1, 0x8000);
  machine_sub.set_ip(0x0001);
  machine_sub.step();
  EXPECT_EQ(machine_sub.rx(0), 0xffff);
  EXPECT_EQ(machine_sub.st().to_uint16(), 0x000e);
}

} // namespace zorita

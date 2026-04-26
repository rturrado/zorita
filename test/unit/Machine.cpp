#include "zorita/Machine.hpp"
#include "zorita/Memory.hpp"

#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace zorita {

const auto res_dir = std::filesystem::path{RESOURCES_DIR};

class MachineTest : public ::testing::Test {
protected:
  Machine machine;
};

TEST_F(MachineTest, Machine) { EXPECT_EQ(machine.state(), State::Stopped); }

TEST_F(MachineTest, load) {
  EXPECT_THAT([&]() { machine.load(res_dir / "does_not_exist.zor"); },
              testing::ThrowsMessage<MachineError>("invalid program path"));
}

TEST_F(MachineTest, step_halt) {
  machine.load(res_dir / "halt.zor");
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.state(), State::Stopped);
}

TEST_F(MachineTest, step_cmp) {
  machine.load(res_dir / "cmp_r0_r1.zor");

  // r0 == r1
  // ZF=1, NF=0, CF=0, OF=0
  machine.set_rx(0, 0x0010);
  machine.set_rx(1, 0x0010);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_TRUE(machine.st().zf());
  EXPECT_FALSE(machine.st().nf());
  EXPECT_FALSE(machine.st().cf());
  EXPECT_FALSE(machine.st().of());

  // r0 > r1
  // ZF=0, NF=0, CF=0, OF=0
  machine.set_rx(0, 0x0010);
  machine.set_rx(1, 0x0001);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_FALSE(machine.st().zf());
  EXPECT_FALSE(machine.st().nf());
  EXPECT_FALSE(machine.st().cf());
  EXPECT_FALSE(machine.st().of());

  // r0 < r1
  // NF is set because signed r0 - r1 is negative
  // CF is set because unsigned r0 - r1 wraps around
  // ZF=0, NF=1, CF=1, OF=0
  machine.set_rx(0, 0x0001);
  machine.set_rx(1, 0x0010);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_FALSE(machine.st().zf());
  EXPECT_TRUE(machine.st().nf());
  EXPECT_TRUE(machine.st().cf());
  EXPECT_FALSE(machine.st().of());

  // Signed overflow: negative minus positive wraps to positive
  // ZF=0, NF=0, CF=0, OF=1
  machine.set_rx(0, 0x8000);
  machine.set_rx(1, 0x7fff);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_FALSE(machine.st().zf());
  EXPECT_FALSE(machine.st().nf());
  EXPECT_FALSE(machine.st().cf());
  EXPECT_TRUE(machine.st().of());

  // Signed overflow: positive minus negative wraps to negative
  // ZF=0, NF=1, CF=1, OF=1
  machine.set_rx(0, 0x7fff);
  machine.set_rx(1, 0x8000);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_FALSE(machine.st().zf());
  EXPECT_TRUE(machine.st().nf());
  EXPECT_TRUE(machine.st().cf());
  EXPECT_TRUE(machine.st().of());
}

TEST_F(MachineTest, step_jmp) {
  machine.set_rx(0, 0x1000);

  machine.load(res_dir / "jmp_zero.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // ZF=0, no jump
  machine.set_ip(0x0001);
  machine.set_st(0x0001);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // ZF=1, jump

  machine.load(res_dir / "jmp_negative.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // NF=0, no jump
  machine.set_ip(0x0001);
  machine.set_st(0x0002);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // NF=1, jump

  machine.load(res_dir / "jmp_carry.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // CF=0, no jump
  machine.set_ip(0x0001);
  machine.set_st(0x0004);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // CF=1, jump

  machine.load(res_dir / "jmp_overflow.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // OF=0, no jump
  machine.set_ip(0x0001);
  machine.set_st(0x0008);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // OF=1, jump

  machine.load(res_dir / "jmp_less_than.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // NF=OF=0, no jump
  machine.set_ip(0x0001);
  machine.set_st(0x0002);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // NF=1, OF=0, jump

  machine.load(res_dir / "jmp_less_or_equal.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // ZF=0, NF=OF=0, no jump
  machine.set_ip(0x0001);
  machine.set_st(0x0001);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // ZF=1, jump

  machine.load(res_dir / "jmp_always.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // always jump

  machine.load(res_dir / "jmp_not_zero.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0001);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // ZF=1, no jump
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // ZF=0, jump

  machine.load(res_dir / "jmp_not_negative.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0002);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // NF=1, no jump
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // NF=0, jump

  machine.load(res_dir / "jmp_not_carry.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0004);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // CF=1, no jump
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // CF=0, jump

  machine.load(res_dir / "jmp_not_overflow.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0008);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // OF=1, no jump
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // OF=0, jump

  machine.load(res_dir / "jmp_greater_or_equal.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0002);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // NF=1, OF=0, no jump
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // NF=OF=0, jump

  machine.load(res_dir / "jmp_greater_than.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0001);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // ZF=1, no jump
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // ZF=0, NF=OF=0, jump

  machine.load(res_dir / "jmp_never.zor");
  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // never jump
}

TEST_F(MachineTest, step_load) {
  machine.load(res_dir / "load_r0_r1.zor");
  machine.set_ip(0x0002);
  machine.set_rx(1, 0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x1234);
}

TEST_F(MachineTest, step_store) {
  machine.load(res_dir / "store_r0_r1.zor");
  machine.set_ip(0x0002);
  machine.set_rx(0, 0x1234);
  machine.set_rx(1, 0x0001);
  machine.step();
  EXPECT_EQ(machine.memory().read(1), 0x1234);
}

TEST_F(MachineTest, step_add) {
  machine.load(res_dir / "add_r0_r0_r1.zor");

  // r0 + r1 == 0
  // ZF=1, NF=0, CF=0, OF=0
  machine.set_rx(0, 0x0000);
  machine.set_rx(1, 0x0000);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x0000);
  EXPECT_TRUE(machine.st().zf());
  EXPECT_FALSE(machine.st().nf());
  EXPECT_FALSE(machine.st().cf());
  EXPECT_FALSE(machine.st().of());

  // r0 + r1 > 0
  // ZF=0, NF=0, CF=0, OF=0
  machine.set_rx(0, 0x0001);
  machine.set_rx(1, 0x0001);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x0002);
  EXPECT_FALSE(machine.st().zf());
  EXPECT_FALSE(machine.st().nf());
  EXPECT_FALSE(machine.st().cf());
  EXPECT_FALSE(machine.st().of());

  // r0 + r1 < 0
  // ZF=0, NF=1, CF=0, OF=0
  machine.set_rx(0, 0x0001);
  machine.set_rx(1, 0xfffe);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0xffff);
  EXPECT_FALSE(machine.st().zf());
  EXPECT_TRUE(machine.st().nf());
  EXPECT_FALSE(machine.st().cf());
  EXPECT_FALSE(machine.st().of());

  // CF is set because unsigned r0 + r1 wraps around
  // ZF=0, NF=0, CF=1, OF=0
  machine.set_rx(0, 0xff00);
  machine.set_rx(1, 0x0101);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x0001);
  EXPECT_FALSE(machine.st().zf());
  EXPECT_FALSE(machine.st().nf());
  EXPECT_TRUE(machine.st().cf());
  EXPECT_FALSE(machine.st().of());

  // Signed overflow: positive plus positive wraps to negative
  // ZF=0, NF=1, CF=0, OF=1
  machine.set_rx(0, 0x7fff);
  machine.set_rx(1, 0x0001);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x8000);
  EXPECT_FALSE(machine.st().zf());
  EXPECT_TRUE(machine.st().nf());
  EXPECT_FALSE(machine.st().cf());
  EXPECT_TRUE(machine.st().of());
}

TEST_F(MachineTest, step_sub) {
  machine.load(res_dir / "sub_r0_r0_r1.zor");

  // r0 == r1
  // ZF=1, NF=0, CF=0, OF=0
  machine.set_rx(0, 0x0010);
  machine.set_rx(1, 0x0010);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x0000);
  EXPECT_TRUE(machine.st().zf());
  EXPECT_FALSE(machine.st().nf());
  EXPECT_FALSE(machine.st().cf());
  EXPECT_FALSE(machine.st().of());

  // r0 > r1
  // ZF=0, NF=0, CF=0, OF=0
  machine.set_rx(0, 0x0010);
  machine.set_rx(1, 0x0001);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x000f);
  EXPECT_FALSE(machine.st().zf());
  EXPECT_FALSE(machine.st().nf());
  EXPECT_FALSE(machine.st().cf());
  EXPECT_FALSE(machine.st().of());

  // r0 < r1
  // NF is set because signed r0 - r1 is negative
  // CF is set because unsigned r0 - r1 wraps around
  // ZF=0, NF=1, CF=1, OF=0
  machine.set_rx(0, 0x0001);
  machine.set_rx(1, 0x0010);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0xfff1);
  EXPECT_FALSE(machine.st().zf());
  EXPECT_TRUE(machine.st().nf());
  EXPECT_TRUE(machine.st().cf());
  EXPECT_FALSE(machine.st().of());

  // Signed overflow: negative minus positive wraps to positive
  // ZF=0, NF=0, CF=0, OF=1
  machine.set_rx(0, 0x8000);
  machine.set_rx(1, 0x7fff);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x0001);
  EXPECT_FALSE(machine.st().zf());
  EXPECT_FALSE(machine.st().nf());
  EXPECT_FALSE(machine.st().cf());
  EXPECT_TRUE(machine.st().of());

  // Signed overflow: positive minus negative wraps to negative
  // ZF=0, NF=1, CF=1, OF=1
  machine.set_rx(0, 0x7fff);
  machine.set_rx(1, 0x8000);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0xffff);
  EXPECT_FALSE(machine.st().zf());
  EXPECT_TRUE(machine.st().nf());
  EXPECT_TRUE(machine.st().cf());
  EXPECT_TRUE(machine.st().of());
}

} // namespace zorita

#include "zorita/Machine.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace zorita {

TEST(MachineTest, default_constructor) {
  Machine machine;
  EXPECT_EQ(machine.state(), State::Stopped);
}

TEST(MachineTest, run_halt) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x0000, // halt
  };
  Machine machine{program};
  machine.run();
  EXPECT_EQ(machine.state(), State::Stopped);
}

TEST(MachineTest, run_fib24) {
  std::vector<uint16_t> rx{
      0x0000, // r0 = 0
      0x0001, // r1 = 1
      0x0018, // r2 = n
      0x0008, // r3 = loop label
      0x0011, // r4 = ret label
      0x0001, // r5 = i = 1
      0x0000, // r6 = lf[0], circular buffer with
      0x0000, // r7 = lf[1], fib(i-1) and fib(i-2)
  };
  std::vector<uint16_t> program{
      // Data section
      0x0003, //   code section
      0x0008, //   loop label
      0x0011, //   ret label

      // Code section
      0x2800, //   cmp n, 0
      0x4100, //   jz ret                  ; jmp ret if n == 0
      0xBC10, //   add lf[1], 0, 1         ; lf[1] = 1
      0x2880, //   cmp n, 1
      0x4100, //   jz ret                  ; jmp ret if n == 1
              // loop:
      0xB690, //   add i, i, 1             ; i++
      0x2A80, //   cmp n, i
      0x4100, //   jz ret                  ; jmp ret if n == i
      0xBB70, //   add lf[0], lf[0], lf[1] ; lf[0] = fib(i)
      0xB690, //   add i, i, 1             ; i++
      0x2A80, //   cmp n, i
      0x4100, //   jz ret                  ; jmp ret if n == i
      0xBF70, //   add lf[1], lf[0], lf[1] ; lf[1] = fib(i)
      0x4EC0, //   j loop                  ; jmp loop
              // ret:
      0xBF70, //   add lf[1], lf[0], lf[1] ; lf[1] = fib(n)
      0x0000, //   halt
  };
  Machine machine{program, Registers{rx}};
  machine.run();
  EXPECT_EQ(machine.rx(7), 46368);
}

TEST(MachineTest, step_halt) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x0000, // halt
  };
  Machine machine{program};
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.state(), State::Stopped);
}

TEST(MachineTest, step_cmp) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x2080, // cmp r0, r1
  };
  Machine machine{program};

  // r0 == r1
  // OF=0, CF=0, NF=0, ZF=1
  machine.set_rx(0, 0x0010);
  machine.set_rx(1, 0x0010);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.st().to_uint16(), 0x0001);

  // r0 > r1
  // OF=0, CF=0, NF=0, ZF=0
  machine.set_rx(0, 0x0010);
  machine.set_rx(1, 0x0001);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.st().to_uint16(), 0x0000);

  // r0 < r1
  // NF is set because signed r0 - r1 is negative
  // CF is set because unsigned r0 - r1 wraps around
  // OF=0, CF=1, NF=1, ZF=0
  machine.set_rx(0, 0x0001);
  machine.set_rx(1, 0x0010);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.st().to_uint16(), 0x0006);

  // Signed overflow: negative minus positive wraps to positive
  // OF=1, CF=0, NF=0, ZF=0
  machine.set_rx(0, 0x8000);
  machine.set_rx(1, 0x7fff);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.st().to_uint16(), 0x0008);

  // Signed overflow: positive minus negative wraps to negative
  // OF=1, CF=1, NF=1, ZF=0
  machine.set_rx(0, 0x7fff);
  machine.set_rx(1, 0x8000);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.st().to_uint16(), 0x000e);
}

TEST(MachineTest, step_jmp_zero) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x4000, // jmp r0 if ZF=1
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // ZF=0, no jump, ip=0x0002

  machine.set_ip(0x0001);
  machine.set_st(0x0001);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // ZF=1, jump, ip=0x1000
}

TEST(MachineTest, step_jmp_negative) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x4200, // jmp r0 if NF=1
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // NF=0, no jump, ip=0x0002

  machine.set_ip(0x0001);
  machine.set_st(0x0002);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // NF=1, jump, ip=0x1000
}

TEST(MachineTest, step_jmp_carry) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x4400, // jmp r0 if CF=1
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // CF=0, no jump, ip=0x0002

  machine.set_ip(0x0001);
  machine.set_st(0x0004);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // CF=1, jump, ip=0x1000
}

TEST(MachineTest, step_jmp_overflow) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x4600, // jmp r0 if OF=1
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // OF=0, no jump, ip=0x0002

  machine.set_ip(0x0001);
  machine.set_st(0x0008);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // OF=1, jump, ip=0x1000
}

TEST(MachineTest, step_jmp_less_than) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x4800, // jmp r0 if NF != OF
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // NF=OF=0, no jump, ip=0x0002

  machine.set_ip(0x0001);
  machine.set_st(0x0002);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // NF=1, OF=0, jump, ip=0x1000
}

TEST(MachineTest, step_jmp_less_or_equal) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x4a00, // jmp r0 if ZF=1 or NF != OF
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // ZF=0, NF=OF=0, no jump, ip=0x0002

  machine.set_ip(0x0001);
  machine.set_st(0x0001);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // ZF=1, jump, ip=0x1000
}

TEST(MachineTest, step_jmp_always) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x4e00, // jmp r0 always
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // always jump, ip=0x1000
}

TEST(MachineTest, step_jmp_not_zero) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x5000, // jmp r0 if ZF=0
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0001);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // ZF=1, no jump, ip=0x0002

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // ZF=0, jump, ip=0x1000
}

TEST(MachineTest, step_jmp_not_negative) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x5200, // jmp r0 if NF=0
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0002);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // NF=1, no jump, ip=0x0002

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // NF=0, jump, ip=0x1000
}

TEST(MachineTest, step_jmp_not_carry) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x5400, // jmp r0 if CF=0
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0004);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // CF=1, no jump, ip=0x0002

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // CF=0, jump, ip=0x1000
}

TEST(MachineTest, step_jmp_not_overflow) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x5600, // jmp r0 if OF=0
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0008);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // OF=1, no jump, ip=0x0002

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // OF=0, jump, ip=0x1000
}

TEST(MachineTest, step_jmp_greater_or_equal) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x5800, // jmp r0 if NF == OF
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0002);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // NF=1, OF=0, no jump, ip=0x0002

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // NF=OF=0, jump, ip=0x1000
}

TEST(MachineTest, step_jmp_greater_than) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x5a00, // jmp r0 if ZF=0 and NF == OF
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0001);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // ZF=1, no jump, ip=0x0002

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x1000); // ZF=0, NF=OF=0, jump, ip=0x1000
}

TEST(MachineTest, step_jmp_never) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0x5e00, // jmp r0 never
  };
  Machine machine{program};
  machine.set_rx(0, 0x1000); // r0=0x1000

  machine.set_ip(0x0001);
  machine.set_st(0x0000);
  machine.step();
  EXPECT_EQ(machine.ip(), 0x0002); // never jump, ip=0x0002
}

TEST(MachineTest, step_load) {
  std::vector<uint16_t> program{
      0x0002, // ip = 0x0002
      0x1234, // address to be read from
      0x6080, // load r0, r1
  };
  Machine machine{program};

  // load r0, r1
  // r1=0x0001, memory[1]=0x1234
  machine.set_ip(0x0002);
  machine.set_rx(1, 0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x1234);
}

TEST(MachineTest, step_store) {
  std::vector<uint16_t> program{
      0x0002, // ip = 0x0002
      0x0000, // address to be written to
      0x8080, // store r0, r1
  };
  Machine machine{program};

  // store r0, r1
  // r0=0x1234, r1=0x0001
  machine.set_ip(0x0002);
  machine.set_rx(0, 0x1234);
  machine.set_rx(1, 0x0001);
  machine.step();
  EXPECT_EQ(machine.memory().read(1), 0x1234);
}

TEST(MachineTest, step_add) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0xa010, // add r0, r0, r1
  };
  Machine machine{program};

  // r0 + r1 == 0
  // OF=0, CF=0, NF=0, ZF=1
  machine.set_rx(0, 0x0000);
  machine.set_rx(1, 0x0000);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x0000);
  EXPECT_EQ(machine.st().to_uint16(), 0x0001);

  // r0 + r1 > 0
  // OF=0, CF=0, NF=0, ZF=0
  machine.set_rx(0, 0x0001);
  machine.set_rx(1, 0x0001);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x0002);
  EXPECT_EQ(machine.st().to_uint16(), 0x0000);

  // r0 + r1 < 0
  // OF=0, CF=0, NF=1, ZF=0
  machine.set_rx(0, 0x0001);
  machine.set_rx(1, 0xfffe);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0xffff);
  EXPECT_EQ(machine.st().to_uint16(), 0x0002);

  // CF is set because unsigned r0 + r1 wraps around
  // OF=0, CF=1, NF=0, ZF=0
  machine.set_rx(0, 0xff00);
  machine.set_rx(1, 0x0101);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x0001);
  EXPECT_EQ(machine.st().to_uint16(), 0x0004);

  // Signed overflow: positive plus positive wraps to negative
  // OF=1, CF=0, NF=1, ZF=0
  machine.set_rx(0, 0x7fff);
  machine.set_rx(1, 0x0001);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x8000);
  EXPECT_EQ(machine.st().to_uint16(), 0x000a);
}

TEST(MachineTest, step_sub) {
  std::vector<uint16_t> program{
      0x0001, // ip = 0x0001
      0xc010, // sub r0, r0, r1
  };
  Machine machine{program};

  // r0 == r1
  // OF=0, CF=0, NF=0, ZF=1
  machine.set_rx(0, 0x0010);
  machine.set_rx(1, 0x0010);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x0000);
  EXPECT_EQ(machine.st().to_uint16(), 0x0001);

  // r0 > r1
  // OF=0, CF=0, NF=0, ZF=0
  machine.set_rx(0, 0x0010);
  machine.set_rx(1, 0x0001);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x000f);
  EXPECT_EQ(machine.st().to_uint16(), 0x0000);

  // r0 < r1
  // NF is set because signed r0 - r1 is negative
  // CF is set because unsigned r0 - r1 wraps around
  // OF=0, CF=1, NF=1, ZF=0
  machine.set_rx(0, 0x0001);
  machine.set_rx(1, 0x0010);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0xfff1);
  EXPECT_EQ(machine.st().to_uint16(), 0x0006);

  // Signed overflow: negative minus positive wraps to positive
  // OF=1, CF=0, NF=0, ZF=0
  machine.set_rx(0, 0x8000);
  machine.set_rx(1, 0x7fff);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0x0001);
  EXPECT_EQ(machine.st().to_uint16(), 0x0008);

  // Signed overflow: positive minus negative wraps to negative
  // OF=1, CF=1, NF=1, ZF=0
  machine.set_rx(0, 0x7fff);
  machine.set_rx(1, 0x8000);
  machine.set_ip(0x0001);
  machine.step();
  EXPECT_EQ(machine.rx(0), 0xffff);
  EXPECT_EQ(machine.st().to_uint16(), 0x000e);
}

} // namespace zorita

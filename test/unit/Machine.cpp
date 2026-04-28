#include "zorita/Machine.hpp"
#include "zorita/Error.hpp"

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

TEST(MachineTest, run_swap) {
  std::vector<uint16_t> rx{
      0x0001, // r0 = address of value A
      0x0002, // r1 = address of value B
  };
  std::vector<uint16_t> program{
      // Data section
      0x0003, // address of code section
      0x00aa, // value A
      0x00bb, // value B

      // Code section
      0x6400, // load r2, r0  ; load r2 with value A from memory
      0x6620, // load r3, r1  ; load r3 with value B from memory
      0x8600, // store r3, r0 ; store value B to memory (where value A was)
      0x8420, // store r2, r1 ; store value A to memory (where value B was)
      0x0000, // halt
  };
  Machine machine{program, Registers{rx}};
  machine.run();
  EXPECT_EQ(machine.memory().read(0x0001), 0x00bb);
  EXPECT_EQ(machine.memory().read(0x0002), 0x00aa);
}

// Calculate fibonacci(n).
// Parameters: n is passed in R2.
//   Use machine.rx(2, val) to initialize R2 to val.
// Return: result in R8.
class FibonacciTest : public ::testing::Test {
protected:
  std::vector<uint16_t> rx_{
      0x0000, // r0 = 0 constant
      0x0001, // r1 = 1 constant
      0x0000, // r2 = n
      0x0006, // r3 = address of loop label
      0x0011, // r4 = address of ret label
      0x0013, // r5 = address of err label
      0x0001, // r6 = i
              // Circular buffer with fib(i-1) and fib(i-2)
      0x0000, // r7 = lf[0]
      0x0000, // r8 = lf[1]
  };
  std::vector<uint16_t> program_{
      // Data section
      0x0001, //   address of code section

      // Code section
      0x2400, //   cmp n, 0
      0x4080, //   jz ret                  ; jmp ret if n == 0
      0xb002, //   add lf[1], 0, 1         ; lf[1] = 1
      0x2420, //   cmp n, 1
      0x4080, //   jz ret                  ; jmp ret if n == 1
              // loop:
      0xacc2, //   add i, i, 1             ; i++
      0x24c0, //   cmp n, i
      0x4080, //   je ret                  ; jmp ret if n == i
      0xaef0, //   add lf[0], lf[0], lf[1] ; lf[0] = fib(i)
      0x46a0, //   je err                  ; jmp err if overflow
      0xacc2, //   add i, i, 1             ; i++
      0x24c0, //   cmp n, i
      0x4080, //   je ret                  ; jmp ret if n == i
      0xb0f0, //   add lf[1], lf[0], lf[1] ; lf[1] = fib(i)
      0x46a0, //   jv err                  ; jmp err if overflow
      0x4e60, //   j loop                  ; jmp loop
              // ret:
      0xb0f0, //   add lf[1], lf[0], lf[1] ; lf[1] = fib(n)
      0x0000, //   halt
              // err:
  };
  Machine machine_{program_, Registers{rx_}};
};

TEST_F(FibonacciTest, fib0) {
  machine_.run();
  EXPECT_EQ(machine_.rx(8), 0);
}

TEST_F(FibonacciTest, fib1) {
  machine_.set_rx(2, 1);
  machine_.run();
  EXPECT_EQ(machine_.rx(8), 1);
}

TEST_F(FibonacciTest, fib10) {
  machine_.set_rx(2, 10);
  machine_.run();
  EXPECT_EQ(machine_.rx(8), 55);
}

TEST_F(FibonacciTest, fib24) {
  machine_.set_rx(2, 24);
  machine_.run();
  EXPECT_EQ(machine_.rx(8), 46368);
}

TEST_F(FibonacciTest, fib25) {
  machine_.set_rx(2, 25);
  EXPECT_THAT([&]() { machine_.run(); },
              testing::ThrowsMessage<DecoderError>("invalid instruction"));
}

// Sum a list of size int16_t values starting at memory position 2.
//   The list can have a maximum of 10 elements.
//   Use memory.write(pos, val) to initialize the list of values.
//   Where: pos can have a value between 2 and 11.
// Parameters: size is passed in memory[2].
//   Use memory.write(2, val) to initialize size to a value between 0 and 10.
// Return: result in R10.
class AccumulatorTest : public ::testing::Test {
protected:
  std::vector<uint16_t> rx_{
      0x0000, // r0 = 0 constant
      0x0001, // r1 = 1 constant
      0x000a, // r2 = 10 constant
      0x0001, // r3 = initialized to the address of size;
              //      later reused as size
      0x0002, // r4 = the address of the list of values
      0x0011, // r5 = address of loop label
      0x0019, // r6 = address of ret label
      0x001a, // r7 = address of err label
      0x0001, // r8 = i
      0x0000, // r9 = val
      0x0000, // r10 = acc
  };
  std::vector<uint16_t> program_{
      // Data section
      0x000c, //   address of code section
      0x0004, //   size of values
      0x1000, //   values[0]
      0x2000, //   values[1]
      0x4000, //   values[2]
      0x7fff, //   values[3]
      0x0000, //   values[4]
      0x0000, //   values[5]
      0x0000, //   values[6]
      0x0000, //   values[7]
      0x0000, //   values[8]
      0x0000, //   values[9]

      // Code section
      0x6660, //   load size, size       ; r3 = memory[r3] = size
      0x2640, //   cmp size, 10          ; cmp r3, r2
      0x5ae0, //   jg err                ; jg r7 (GreaterThan=13)
      0x2600, //   cmp size, 0           ; cmp r3, r0
      0x40c0, //   jz ret                ; jz r6
              // loop:
      0x7280, //   load val, vptr        ; r9 = memory[r4]
      0xb552, //   add acc, acc, val     ; r10 = r10 + r9
      0x46e0, //   jv err                ; jv r7
      0x3060, //   cmp i, size           ; cmp r8, r3
      0x40c0, //   jz ret                ; jz r6
      0xb102, //   add i, i, 1           ; r8 = r8 + r1
      0xa882, //   add vptr, vptr, 1     ; r4 = r4 + r1
      0x4ea0, //   j loop                ; j r5 (Always=7)
              // ret:
      0x0000, //   halt
              // err:
  };
  Machine machine_{program_, Registers{rx_}};
};

TEST_F(AccumulatorTest, size_of_values_11) {
  machine_.memory().write(1, 11); // size of values = 11
  EXPECT_THAT([&]() { machine_.run(); },
              testing::ThrowsMessage<DecoderError>("invalid instruction"));
}

TEST_F(AccumulatorTest, size_of_values_0) {
  machine_.memory().write(1, 0); // size of values = 0
  machine_.run();
  EXPECT_EQ(machine_.rx(10), 0);
}

TEST_F(AccumulatorTest, valid_sum) {
  machine_.memory().write(1, 2); // size of values = 2
  machine_.run();
  EXPECT_EQ(machine_.rx(10), 0x3000);
}

TEST_F(AccumulatorTest, sum_overflows) {
  // size of values = 4
  EXPECT_THAT([&]() { machine_.run(); },
              testing::ThrowsMessage<DecoderError>("invalid instruction"));
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
      0x2020, // cmp r0, r1
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
      0x6020, // load r0, r1
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
      0x8020, // store r0, r1
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
      0xa002, // add r0, r0, r1
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
      0xc002, // sub r0, r0, r1
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

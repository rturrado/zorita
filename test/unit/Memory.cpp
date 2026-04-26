#include "zorita/Memory.hpp"
#include "zorita/Error.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>

#include <fmt/format.h>

namespace zorita {

class MemoryTest : public ::testing::Test {
protected:
  Memory memory;
};

TEST_F(MemoryTest, default_constructor) {
  EXPECT_EQ(memory.read(0x0000), 0xffff);
  EXPECT_EQ(memory.read(0x8000), 0xffff);
  EXPECT_EQ(memory.read(0xffff), 0xffff);
}

TEST(Memory, construct_from_data) {
  std::vector<uint16_t> data(MEMORY_SIZE + 1, 0x0000);
  EXPECT_THAT(
      [&]() { Memory{data}; },
      testing::ThrowsMessage<MemoryError>(fmt::format(
          "invalid program: size is bigger than {} bytes", MEMORY_SIZE * 2)));
}

TEST_F(MemoryTest, read_after_write) {
  memory.write(0, 0x1234);
  EXPECT_EQ(memory.read(0), 0x1234);
}

} // namespace zorita

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

TEST_F(MemoryTest, load) {
  auto res_dir = std::filesystem::path{RESOURCES_DIR};
  EXPECT_THAT([&]() { memory.load(res_dir / "size_3_bytes.zor"); },
              testing::ThrowsMessage<MemoryError>(
                  "invalid program: size is not aligned to a 2-byte word"));
  EXPECT_THAT(
      [&]() { memory.load(res_dir / "size_65537_words.zor"); },
      testing::ThrowsMessage<MemoryError>(fmt::format(
          "invalid program: size is bigger than {} bytes", MEMORY_SIZE * 2)));
  memory.load(res_dir / "halt.zor");
  EXPECT_EQ(memory.read(0), 0x0001);
  EXPECT_EQ(memory.read(1), 0x0000);
  EXPECT_EQ(memory.read(2), 0xffff);
}

TEST_F(MemoryTest, read_after_write) {
  memory.write(0, 0x1234);
  EXPECT_EQ(memory.read(0), 0x1234);
}

} // namespace zorita

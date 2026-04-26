#include "zorita/Memory.hpp"

#include "zorita/Error.hpp"

#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <vector>

namespace zorita {

Memory::Memory() { data_.fill(0xffff); }

Memory::Memory(const std::vector<uint16_t> &data) {
  if (data.size() > data_.size()) {
    throw MemoryError{fmt::format(
        "invalid program: size is bigger than {} bytes", data_.size() * 2)};
  }
  std::copy(data.begin(), data.end(), data_.begin());
  std::fill(data_.begin() + data.size(), data_.end(), 0xffff);
}

void Memory::load(const std::filesystem::path &program) {
  // Get size of the program in bytes
  auto size_in_bytes = std::filesystem::file_size(program);
  // Check program size
  if (size_in_bytes > data_.size() * 2) {
    throw MemoryError{fmt::format(
        "invalid program: size is bigger than {} bytes", data_.size() * 2)};
  }
  if (size_in_bytes % 2) {
    throw MemoryError{"invalid program: size is not aligned to a 2-byte word"};
  }
  // Open program in binary mode and for reading
  auto fs = std::ifstream{program, std::ios::binary | std::ios::in};
  // Load the program into a temporary buffer
  fs.read(reinterpret_cast<char *>(data_.data()),
          size_in_bytes); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
}

uint16_t Memory::read(uint16_t address) { return data_[address]; }

void Memory::write(uint16_t address, uint16_t value) { data_[address] = value; }

} // namespace zorita

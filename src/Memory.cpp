#include "zorita/Memory.hpp"

#include "zorita/Error.hpp"

#include <algorithm>
#include <filesystem>
#include <fmt/format.h>
#include <vector>

namespace zorita {

Memory::Memory() { data_.fill(0xffff); }

Memory::Memory(const std::vector<uint16_t> &data) {
  if (data.size() > data_.size()) {
    throw MemoryError{fmt::format(
        "invalid program: size is bigger than {} bytes", data_.size() * 2)};
  }
  std::ranges::copy(data, data_.begin());
  std::fill(std::next(data_.begin(), static_cast<ptrdiff_t>(data.size())),
            data_.end(), 0xffff);
}

uint16_t Memory::read(uint16_t address) { return data_.at(address); }

void Memory::write(uint16_t address, uint16_t value) {
  data_.at(address) = value;
}

} // namespace zorita

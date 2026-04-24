#pragma once

#include "zorita/Registers.hpp"

#include <cstdint>

namespace zorita {

/// @brief Condition opcode.
enum class ConditionOpcode : uint8_t {
  Zero = 0,
  Negative = 1,
  Overflow = 2,
  LessThan = 3,
  LessOrEqual = 4,
  // Reserved = 5,
  // Reserved = 6,
  Always = 7,
  NotZero = 8,
  NotNegative = 9,
  NotOverflow = 10,
  GreaterOrEqual = 11,
  GreaterThan = 12,
  // Reserved = 13,
  // Reserved = 14,
  Never = 15,
};

/// @brief Encapsulates a condition opcode and evaluates it against a status
/// register.
class Condition {
public:
  /// @brief Constructs a Condition from a condition opcode.
  /// @note Constructor is not explicit, so uint8_t types can also be used.
  /// @param opcode Condition opcode value.
  Condition(ConditionOpcode opcode);

  /// @brief Evaluates the condition against a status register.
  /// @param st The status register to test.
  /// @return True if the condition holds.
  /// @throws ConditionError if the condition code is unrecognized.
  bool check(const StatusRegister &st) const;

private:
  ConditionOpcode opcode_;
};

} // namespace zorita

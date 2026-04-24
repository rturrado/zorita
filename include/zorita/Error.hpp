#pragma once

#include <stdexcept>
#include <string>

namespace zorita {

/// @brief Base error class for all Zorita machine errors.
struct MachineError : std::runtime_error {
  explicit MachineError(const std::string &message)
      : std::runtime_error{message} {}
};

/// @brief Thrown when an invalid or unsupported condition code is evaluated.
struct ConditionError : MachineError {
  explicit ConditionError(const std::string &message) : MachineError{message} {}
};

/// @brief Thrown when a memory word cannot be decoded into a valid instruction.
struct DecoderError : MachineError {
  explicit DecoderError(const std::string &message) : MachineError{message} {}
};

} // namespace zorita

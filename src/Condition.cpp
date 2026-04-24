#include "zorita/Condition.hpp"

#include "zorita/Error.hpp"

namespace zorita {

Condition::Condition(ConditionOpcode opcode) : opcode_{opcode} {}

bool Condition::check(const StatusRegister &st) const {
  switch (opcode_) {
  case ConditionOpcode::Zero:
    return st.zf();
  case ConditionOpcode::Negative:
    return st.nf();
  case ConditionOpcode::Overflow:
    return st.of();
  case ConditionOpcode::LessThan:
    return st.nf() != st.of();
  case ConditionOpcode::LessOrEqual:
    return st.nf() != st.of() || st.zf();
  case ConditionOpcode::Always:
    return true;
  case ConditionOpcode::NotZero:
    return !st.zf();
  case ConditionOpcode::NotNegative:
    return !st.nf();
  case ConditionOpcode::NotOverflow:
    return !st.of();
  case ConditionOpcode::GreaterOrEqual:
    return st.nf() == st.of();
  case ConditionOpcode::GreaterThan:
    return st.nf() == st.of() && !st.zf();
  case ConditionOpcode::Never:
    return false;
  default:
    throw ConditionError{"invalid condition code"};
  }
}

} // namespace zorita

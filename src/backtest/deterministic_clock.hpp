#pragma once

#include "core/time/time.hpp"

namespace backtest {

class DeterministicClock {
public:
  explicit DeterministicClock(core::time::TimePoint start) : current_(start) {}

  core::time::TimePoint now() const { return current_; }
  void set(core::time::TimePoint tp) { current_ = tp; }

private:
  core::time::TimePoint current_{};
};

} // namespace backtest

#pragma once

#include "core/time/time.hpp"

namespace core::time {

class Clock {
public:
  virtual ~Clock() = default;
  virtual TimePoint now() const = 0;
};

class SystemClock final : public Clock {
public:
  TimePoint now() const override;
};

class SimClock final : public Clock {
public:
  explicit SimClock(TimePoint start) : current_(start) {}

  TimePoint now() const override { return current_; }
  void set(TimePoint tp) { current_ = tp; }

private:
  TimePoint current_{};
};

} // namespace core::time

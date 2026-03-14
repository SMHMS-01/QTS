#pragma once

#include "core/types/types.hpp"

namespace core::time {

enum class ClockType : std::uint8_t {
  Exchange,
  Receive,
  Local
};

struct TimePoint {
  core::types::TimestampNs ts;
  ClockType type = ClockType::Local;
};

inline constexpr bool operator==(TimePoint a, TimePoint b) {
  return a.type == b.type && a.ts == b.ts;
}

inline constexpr bool operator<(TimePoint a, TimePoint b) {
  if (a.type != b.type) {
    return static_cast<std::uint8_t>(a.type) < static_cast<std::uint8_t>(b.type);
  }
  return a.ts < b.ts;
}

} // namespace core::time

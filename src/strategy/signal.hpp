#pragma once

#include "core/time/time.hpp"
#include "core/types/types.hpp"

namespace strategy {

enum class Side : std::uint8_t {
  Buy,
  Sell
};

struct Signal {
  core::types::SymbolId symbol;
  Side side = Side::Buy;
  double strength = 0.0;
  core::time::TimePoint time;
};

} // namespace strategy

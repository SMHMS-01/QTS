#pragma once

#include "core/bus/event.hpp"
#include "core/bus/event_bus.hpp"
#include "backtest/deterministic_clock.hpp"
#include "market/feed/replay.hpp"

namespace backtest {

class ReplayEngine {
public:
  ReplayEngine(backtest::DeterministicClock* clock,
               market::feed::ReplaySource* source,
               core::bus::EventBus* bus)
      : clock_(clock), source_(source), bus_(bus) {}

  bool step();
  core::time::TimePoint current_time() const { return last_time_; }

private:
  backtest::DeterministicClock* clock_ = nullptr;
  market::feed::ReplaySource* source_ = nullptr;
  core::bus::EventBus* bus_ = nullptr;
  core::time::TimePoint last_time_{};
};

} // namespace backtest

#pragma once

#include "core/bus/event.hpp"
#include "core/time/clock.hpp"
#include "market/feed/replay.hpp"

namespace backtest {

class ReplayController {
public:
  ReplayController(core::time::SimClock* clock, core::bus::EventBus* bus)
      : clock_(clock), bus_(bus) {}

  void configure(const market::feed::ReplayConfig& cfg) { config_ = cfg; }
  bool step();

  void set_source(market::feed::ReplaySource* source) { source_ = source; }

private:
  core::time::SimClock* clock_ = nullptr;
  core::bus::EventBus* bus_ = nullptr;
  market::feed::ReplayConfig config_{};
  market::feed::ReplaySource* source_ = nullptr;
};

} // namespace backtest

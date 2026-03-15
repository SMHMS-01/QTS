#pragma once

#include "core/bus/event.hpp"
#include "strategy/portfolio.hpp"
#include "strategy/strategy.hpp"

namespace strategy {

class StrategyRunner {
public:
  StrategyRunner(IStrategy* strategy, Portfolio* portfolio, core::bus::EventBus* bus)
      : strategy_(strategy), portfolio_(portfolio), bus_(bus) {}

  void on_event(const core::bus::Event& event) {
    if (strategy_) {
      strategy_->on_event(event);
    }
  }

  void step() {
    if (strategy_) {
      strategy_->emit_signals();
    }
  }

private:
  IStrategy* strategy_ = nullptr;
  Portfolio* portfolio_ = nullptr;
  core::bus::EventBus* bus_ = nullptr;
};

} // namespace strategy

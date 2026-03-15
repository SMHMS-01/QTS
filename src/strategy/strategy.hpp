#pragma once

#include "core/bus/event.hpp"
#include "strategy/signal.hpp"

namespace strategy {

class IStrategy {
public:
  virtual ~IStrategy() = default;
  virtual void on_event(const core::bus::Event& event) = 0;
  virtual void emit_signals() = 0;
};

} // namespace strategy

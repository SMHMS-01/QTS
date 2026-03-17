#include "strategy/strategy.hpp"

namespace strategy {

class DummyStrategy final : public IStrategy {
public:
  void on_event(const core::bus::Event&) override {}
  void emit_signals() override {}
};

} // namespace strategy

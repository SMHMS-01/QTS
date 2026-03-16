#include "backtest/replay_controller.hpp"

namespace backtest {

bool ReplayController::step() {
  if (!clock_ || !bus_ || !source_) {
    return false;
  }

  if (!source_->next()) {
    return false;
  }

  if (const auto* header = source_->peek_header()) {
    clock_->set(header->receive_time);
  }

  core::bus::Event event{};
  if (const auto* header = source_->peek_header()) {
    event.time = header->receive_time;
  }
  event.type = core::bus::EventType::MarketData;
  event.payload = const_cast<void*>(source_->payload());

  bus_->publish(event);
  return true;
}

} // namespace backtest

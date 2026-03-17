#include "backtest/replay_engine.hpp"

namespace backtest {

bool ReplayEngine::step() {
  if (!clock_ || !source_ || !bus_) {
    return false;
  }

  if (!source_->next()) {
    return false;
  }

  if (const auto* header = source_->peek_header()) {
    clock_->set(header->receive_time);
    last_time_ = header->receive_time;
  }

  core::bus::Event event{};
  if (const auto* header = source_->peek_header()) {
    event.time = header->receive_time;
    event.sequence = header->sequence;
  }
  event.type = core::bus::EventType::MarketData;
  event.payload = const_cast<void*>(source_->payload());

  bus_->publish(event);
  return true;
}

} // namespace backtest

#include <iostream>

#include "backtest/deterministic_clock.hpp"
#include "backtest/replay_engine.hpp"
#include "core/bus/event_bus.hpp"
#include "market/feed/replay.hpp"

namespace {

class CountingSink final : public core::bus::IEventSink {
public:
  void on_event(const core::bus::Event&) override { ++count; }
  int count = 0;
};

class DummySource final : public market::feed::ReplaySource {
public:
  explicit DummySource(int total) : total_(total) {}

  const market::feed::FeedHeader* peek_header() const override { return &header_; }

  bool next() override {
    if (index_ >= total_) {
      return false;
    }
    header_.sequence = static_cast<std::uint64_t>(index_ + 1);
    ++index_;
    return true;
  }

  const void* payload() const override { return nullptr; }

  market::feed::FeedMessageType payload_type() const override {
    return market::feed::FeedMessageType::BookUpdate;
  }

private:
  int total_ = 0;
  int index_ = 0;
  mutable market::feed::FeedHeader header_{};
};

} // namespace

int main() {
  backtest::DeterministicClock clock({});
  core::bus::EventBus bus;
  CountingSink sink;
  bus.subscribe(core::bus::EventType::MarketData, &sink);

  DummySource source(5);
  backtest::ReplayEngine engine(&clock, &source, &bus);

  int steps = 0;
  while (engine.step()) {
    ++steps;
  }

  std::cout << "Backtest demo finished. Steps=" << steps
            << ", Events=" << sink.count << "\n";
  return 0;
}

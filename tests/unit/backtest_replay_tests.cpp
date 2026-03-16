#include <cassert>

#include "backtest/replay_controller.hpp"
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
  core::bus::EventBus bus;
  CountingSink sink;
  bus.subscribe(core::bus::EventType::MarketData, &sink);

  backtest::ReplayController controller(new core::time::SimClock({}), &bus);
  DummySource source(3);
  controller.set_source(&source);

  assert(controller.step());
  assert(controller.step());
  assert(controller.step());
  assert(!controller.step());
  assert(sink.count == 3);

  return 0;
}

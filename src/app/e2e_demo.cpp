#include <iostream>

#include "backtest/deterministic_clock.hpp"
#include "backtest/pnl.hpp"
#include "backtest/replay_engine.hpp"
#include "core/bus/event_bus.hpp"
#include "market/feed/replay.hpp"
#include "market/normalization/normalization.hpp"
#include "market/normalization/sequence_guard.hpp"
#include "market/order_book/order_book.hpp"
#include "strategy/strategy.hpp"

namespace {

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

class OrderBookSink final : public core::bus::IEventSink {
public:
  void on_event(const core::bus::Event&) override {
    book.apply_update(market::order_book::Side::Bid, {100}, {10});
    book.apply_update(market::order_book::Side::Ask, {101}, {7});
  }

  market::order_book::OrderBookL2 book;
};

} // namespace

int main() {
  market::normalization::SequenceGuard guard;
  backtest::DeterministicClock clock({});
  core::bus::EventBus bus;

  DummySource source(5);
  backtest::ReplayEngine engine(&clock, &source, &bus);

  OrderBookSink book_sink;
  bus.subscribe(core::bus::EventType::MarketData, &book_sink);

  int steps = 0;
  while (engine.step()) {
    ++steps;
  }

  market::order_book::Level bid{};
  market::order_book::Level ask{};
  book_sink.book.best_bid(bid);
  book_sink.book.best_ask(ask);

  std::cout << "E2E demo finished. Steps=" << steps << "\n";
  std::cout << "Best Bid: " << bid.price.value << " x " << bid.quantity.value << "\n";
  std::cout << "Best Ask: " << ask.price.value << " x " << ask.quantity.value << "\n";

  return 0;
}

#include <iostream>

#include "backtest/deterministic_clock.hpp"
#include "backtest/replay_engine.hpp"
#include "core/bus/event_bus.hpp"
#include "execution/order_manager.hpp"
#include "market/feed/replay.hpp"
#include "market/order_book/order_book.hpp"
#include "risk/risk.hpp"
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

class DummyStrategy final : public strategy::IStrategy {
public:
  void on_event(const core::bus::Event&) override {}
  void emit_signals() override { emitted = true; }
  bool emitted = false;
};

} // namespace

int main() {
  backtest::DeterministicClock clock({});
  core::bus::EventBus bus;

  DummySource source(3);
  backtest::ReplayEngine engine(&clock, &source, &bus);

  OrderBookSink book_sink;
  bus.subscribe(core::bus::EventType::MarketData, &book_sink);

  int steps = 0;
  while (engine.step()) {
    ++steps;
  }

  market::order_book::Level bid{};
  market::order_book::Level ask{};
  const bool has_bid = book_sink.book.best_bid(bid);
  const bool has_ask = book_sink.book.best_ask(ask);

  DummyStrategy strat;
  strat.emit_signals();

  std::cout << "System demo finished. Steps=" << steps << "\n";
  if (has_bid) {
    std::cout << "Best Bid: " << bid.price.value << " x " << bid.quantity.value << "\n";
  }
  if (has_ask) {
    std::cout << "Best Ask: " << ask.price.value << " x " << ask.quantity.value << "\n";
  }
  std::cout << "Strategy emitted: " << (strat.emitted ? "yes" : "no") << "\n";

  return 0;
}

#include <cassert>
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

struct BookUpdateMsg {
  market::order_book::Side side;
  core::types::PriceTicks price;
  core::types::QuantityLots qty;
};

class DummySource final : public market::feed::ReplaySource {
public:
  DummySource() = default;

  const market::feed::FeedHeader* peek_header() const override { return &header_; }

  bool next() override {
    if (index_ >= static_cast<int>(std::size(msgs_))) {
      return false;
    }
    header_.sequence = sequences_[index_];
    header_.exchange_time.ts.value = header_.sequence;
    header_.receive_time.ts.value = header_.sequence;
    ++index_;
    return true;
  }

  const void* payload() const override { return &msgs_[index_ - 1]; }

  market::feed::FeedMessageType payload_type() const override {
    return market::feed::FeedMessageType::BookUpdate;
  }

private:
  int index_ = 0;
  mutable market::feed::FeedHeader header_{};
  BookUpdateMsg msgs_[5] = {
      {market::order_book::Side::Bid, {100}, {10}},
      {market::order_book::Side::Ask, {101}, {7}},
      {market::order_book::Side::Bid, {99}, {5}},
      {market::order_book::Side::Ask, {102}, {3}},
      {market::order_book::Side::Bid, {100}, {0}},
  };
  std::uint64_t sequences_[5] = {1, 2, 4, 5, 6};
};

class OrderBookSink final : public core::bus::IEventSink {
public:
  explicit OrderBookSink(market::normalization::SequenceGuard* guard) : guard_(guard) {
    assert(guard_ != nullptr);
  }

  void on_event(const core::bus::Event& e) override {
    // SequenceGuard behavior: after a gap, all subsequent updates are rejected
    // until an explicit reset (Strategy A).
    if (!guard_->accept(e.sequence)) {
      return;
    }
    const auto* msg = static_cast<const BookUpdateMsg*>(e.payload);
    if (!msg) {
      return;
    }
    book.apply_update(msg->side, msg->price, msg->qty);
  }

  market::normalization::SequenceGuard* guard_ = nullptr;
  market::order_book::OrderBookL2 book;
};

} // namespace

int main() {
  market::normalization::SequenceGuard guard;
  backtest::DeterministicClock clock({});
  core::bus::EventBus bus;

  DummySource source;
  backtest::ReplayEngine engine(&clock, &source, &bus);

  OrderBookSink book_sink(&guard);
  bus.subscribe(core::bus::EventType::MarketData, &book_sink);

  int steps = 0;
  while (engine.step()) {
    ++steps;
  }
  assert(steps == 5);

  market::order_book::Level bid{};
  market::order_book::Level ask{};
  book_sink.book.best_bid(bid);
  book_sink.book.best_ask(ask);

  std::cout << "E2E demo finished. Steps=" << steps << "\n";
  std::cout << "Best Bid: " << bid.price.value << " x " << bid.quantity.value << "\n";
  std::cout << "Best Ask: " << ask.price.value << " x " << ask.quantity.value << "\n";
  assert(bid.price.value == 100);
  assert(bid.quantity.value == 10);
  assert(ask.price.value == 101);
  assert(ask.quantity.value == 7);

  return 0;
}

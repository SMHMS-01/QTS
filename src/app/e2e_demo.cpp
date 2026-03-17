#include <cassert>
#include <iostream>

#include "backtest/deterministic_clock.hpp"
#include "backtest/pnl.hpp"
#include "backtest/replay_engine.hpp"
#include "core/bus/event_bus.hpp"
#include "market/feed/replay.hpp"
#include "market/feed/tardis_incremental_book_l2_csv.hpp"
#include "market/normalization/normalization.hpp"
#include "market/normalization/sequence_guard.hpp"
#include "market/normalization/symbol_map.hpp"
#include "market/order_book/order_book.hpp"
#include "observability/logging.hpp"
#include "observability/metrics.hpp"
#include "observability/tracing.hpp"
#include "strategy/strategy.hpp"

namespace {

struct BookUpdateMsg {
  core::types::SymbolId symbol;
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
      {{1}, market::order_book::Side::Bid, {100}, {10}},
      {{1}, market::order_book::Side::Ask, {101}, {7}},
      {{1}, market::order_book::Side::Bid, {99}, {5}},
      {{1}, market::order_book::Side::Ask, {102}, {3}},
      {{1}, market::order_book::Side::Bid, {100}, {0}},
  };
  std::uint64_t sequences_[5] = {1, 2, 4, 5, 6};
};

class OrderBookSink final : public core::bus::IEventSink {
public:
  OrderBookSink(market::normalization::SequenceGuard* guard,
                observability::ILogger* logger,
                bool use_depth_update)
      : guard_(guard), use_depth_update_(use_depth_update) {
    assert(guard_ != nullptr);
    book.set_logger(logger);
  }

  void on_event(const core::bus::Event& e) override {
    // SequenceGuard behavior: after a gap, all subsequent updates are rejected
    // until an explicit reset (Strategy A).
    if (!guard_->accept(e.sequence)) {
      return;
    }
    if (use_depth_update_) {
      const auto* msg = static_cast<const market::feed::DepthUpdate*>(e.payload);
      if (!msg) {
        return;
      }
      if (msg->is_snapshot && msg->header.exchange_time.ts.value != last_snapshot_ts_) {
        book.clear();
        last_snapshot_ts_ = msg->header.exchange_time.ts.value;
      }
      market::order_book::UpdateContext ctx{msg->header.symbol, e.sequence};
      book.apply_update(msg->is_bid ? market::order_book::Side::Bid
                                    : market::order_book::Side::Ask,
                        msg->price,
                        msg->quantity,
                        &ctx);
      return;
    }

    const auto* msg = static_cast<const BookUpdateMsg*>(e.payload);
    if (!msg) {
      return;
    }
    market::order_book::UpdateContext ctx{msg->symbol, e.sequence};
    book.apply_update(msg->side, msg->price, msg->qty, &ctx);
  }

  market::normalization::SequenceGuard* guard_ = nullptr;
  market::order_book::OrderBookL2 book;
  bool use_depth_update_ = false;
  std::int64_t last_snapshot_ts_ = -1;
};

} // namespace

int main(int argc, char** argv) {
  market::normalization::SequenceGuard guard;
  backtest::DeterministicClock clock({});
  core::bus::EventBus bus;
  observability::StdoutLogger logger;
  observability::StdoutMetrics metrics;
  observability::TracerImpl tracer;
  backtest::PnLTracker pnl(&metrics);

  const bool use_real_data = (argc > 1);
  market::normalization::SimpleStringSymbolMap symbol_map;
  market::normalization::IdentityTimeAligner time_aligner;

  DummySource dummy_source;
  market::feed::TardisIncrementalBookL2CsvReplay real_source(
      use_real_data ? argv[1] : "", &symbol_map, &time_aligner);
  market::feed::ReplaySource* source =
      use_real_data ? static_cast<market::feed::ReplaySource*>(&real_source)
                    : static_cast<market::feed::ReplaySource*>(&dummy_source);

  backtest::ReplayEngine engine(&clock, source, &bus);

  OrderBookSink book_sink(&guard, &logger, use_real_data);
  bus.subscribe(core::bus::EventType::MarketData, &book_sink);

  int steps = 0;
  while (engine.step()) {
    const auto span = tracer.start_span("replay.step");
    ++steps;
    market::order_book::Level bid{};
    if (book_sink.book.best_bid(bid)) {
      if (steps == 1) {
        pnl.on_fill({1}, {1}, bid.price);
      }
      pnl.mark_to_market({1}, bid.price);
    }
    tracer.end_span(span);
  }
  if (use_real_data) {
    assert(steps > 0);
  } else {
    assert(steps == 5);
  }

  market::order_book::Level bid{};
  market::order_book::Level ask{};
  book_sink.book.best_bid(bid);
  book_sink.book.best_ask(ask);

  std::cout << "E2E demo finished. Steps=" << steps << "\n";
  if (use_real_data) {
    const double bid_px = static_cast<double>(bid.price.value) /
                          market::feed::TardisIncrementalBookL2CsvReplay::kPriceScale;
    const double bid_qty = static_cast<double>(bid.quantity.value) /
                           market::feed::TardisIncrementalBookL2CsvReplay::kAmountScale;
    const double ask_px = static_cast<double>(ask.price.value) /
                          market::feed::TardisIncrementalBookL2CsvReplay::kPriceScale;
    const double ask_qty = static_cast<double>(ask.quantity.value) /
                           market::feed::TardisIncrementalBookL2CsvReplay::kAmountScale;
    std::cout << "Best Bid: " << bid_px << " x " << bid_qty << "\n";
    std::cout << "Best Ask: " << ask_px << " x " << ask_qty << "\n";
    assert(bid.price.value <= ask.price.value);
  } else {
    std::cout << "Best Bid: " << bid.price.value << " x " << bid.quantity.value << "\n";
    std::cout << "Best Ask: " << ask.price.value << " x " << ask.quantity.value << "\n";
    assert(bid.price.value == 100);
    assert(bid.quantity.value == 10);
    assert(ask.price.value == 101);
    assert(ask.quantity.value == 7);
  }

  return 0;
}

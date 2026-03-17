#include <cassert>
#include <fstream>
#include <iostream>

#include "backtest/deterministic_clock.hpp"
#include "backtest/pnl.hpp"
#include "backtest/replay_engine.hpp"
#include "core/bus/event_bus.hpp"
#include "market/feed/binance_depth_csv.hpp"
#include "market/feed/tardis_incremental_book_l2_csv.hpp"
#include "market/normalization/sequence_guard.hpp"
#include "market/normalization/symbol_map.hpp"
#include "market/order_book/order_book.hpp"
#include "observability/logging.hpp"
#include "observability/metrics.hpp"

namespace {

class OrderBookSink final : public core::bus::IEventSink {
 public:
  OrderBookSink(market::normalization::SequenceGuard* guard,
                observability::ILogger* logger, bool use_depth_update)
      : guard_(guard), logger_(logger), use_depth_update_(use_depth_update) {
    assert(guard_ != nullptr);
    // book_.set_logger(logger_);
  }

  void on_event(const core::bus::Event& e) override {
    if (!guard_->accept(e.sequence)) {
      return;
    }
    if (use_depth_update_) {
      const auto* msg =
          static_cast<const market::feed::DepthUpdate*>(e.payload);
      if (!msg) {
        return;
      }
      if (msg->is_snapshot &&
          msg->header.exchange_time.ts.value != last_snapshot_ts_) {
        book_.clear();
        last_snapshot_ts_ = msg->header.exchange_time.ts.value;
      }
      market::order_book::UpdateContext ctx{msg->header.symbol, e.sequence};
      book_.apply_update(msg->is_bid ? market::order_book::Side::Bid
                                     : market::order_book::Side::Ask,
                         msg->price, msg->quantity, &ctx);
      return;
    }

    const auto* msg = static_cast<const market::feed::BookUpdate*>(e.payload);
    if (!msg) {
      return;
    }
    market::order_book::UpdateContext ctx{msg->header.symbol, e.sequence};
    book_.apply_update(msg->is_bid ? market::order_book::Side::Bid
                                   : market::order_book::Side::Ask,
                       msg->price, msg->quantity, &ctx);
  }

  market::order_book::OrderBookL2& book() { return book_; }

 private:
  market::normalization::SequenceGuard* guard_ = nullptr;
  observability::ILogger* logger_ = nullptr;
  market::order_book::OrderBookL2 book_;
  bool use_depth_update_ = false;
  std::int64_t last_snapshot_ts_ = -1;
};

}  // namespace

namespace {

bool looks_like_tardis(std::string_view path) {
  std::ifstream file{std::string(path)};
  if (!file.is_open()) {
    return false;
  }
  std::string line;
  if (!std::getline(file, line)) {
    return false;
  }
  return line.rfind("exchange,", 0) == 0;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: binance_replay_demo <csv_path>\n";
    return 1;
  }

  const std::string path = argv[1];
  const bool use_tardis = looks_like_tardis(path);
  observability::StdoutLogger logger;
  observability::StdoutMetrics metrics;
  backtest::PnLTracker pnl(&metrics);

  market::normalization::SimpleStringSymbolMap symbol_map;
  market::normalization::IdentityTimeAligner time_aligner;
  market::feed::BinanceDepthCsvReplay binance_source(path, &symbol_map,
                                                     &time_aligner);
  market::feed::TardisIncrementalBookL2CsvReplay tardis_source(
      path, &symbol_map, &time_aligner);
  market::feed::ReplaySource* source =
      use_tardis ? static_cast<market::feed::ReplaySource*>(&tardis_source)
                 : static_cast<market::feed::ReplaySource*>(&binance_source);

  backtest::DeterministicClock clock({});
  core::bus::EventBus bus;
  backtest::ReplayEngine engine(&clock, source, &bus);

  market::normalization::SequenceGuard guard;
  OrderBookSink book_sink(&guard, &logger, use_tardis);
  bus.subscribe(core::bus::EventType::MarketData, &book_sink);

  int steps = 0;
  while (engine.step()) {
    ++steps;
    market::order_book::Level bid{};
    if (book_sink.book().best_bid(bid)) {
      if (steps == 1) {
        pnl.on_fill({1}, {1}, bid.price);
      }
      pnl.mark_to_market({1}, bid.price);
    }
  }

  market::order_book::Level bid{};
  market::order_book::Level ask{};
  if (book_sink.book().best_bid(bid)) {
    if (use_tardis) {
      const double bid_px =
          static_cast<double>(bid.price.value) /
          market::feed::TardisIncrementalBookL2CsvReplay::kPriceScale;
      const double bid_qty =
          static_cast<double>(bid.quantity.value) /
          market::feed::TardisIncrementalBookL2CsvReplay::kAmountScale;
      std::cout << "Best Bid: " << bid_px << " x " << bid_qty << "\n";
    } else {
      std::cout << "Best Bid: " << bid.price.value << " x "
                << bid.quantity.value << "\n";
    }
  }
  if (book_sink.book().best_ask(ask)) {
    if (use_tardis) {
      const double ask_px =
          static_cast<double>(ask.price.value) /
          market::feed::TardisIncrementalBookL2CsvReplay::kPriceScale;
      const double ask_qty =
          static_cast<double>(ask.quantity.value) /
          market::feed::TardisIncrementalBookL2CsvReplay::kAmountScale;
      std::cout << "Best Ask: " << ask_px << " x " << ask_qty << "\n";
    } else {
      std::cout << "Best Ask: " << ask.price.value << " x "
                << ask.quantity.value << "\n";
    }
  }
  std::cout << "Replay finished. Steps=" << steps << "\n";
  if (steps == 0) {
    if (use_tardis) {
      std::cerr << "Parse error: " << tardis_source.last_error() << "\n";
    } else {
      std::cerr << "Parse error: " << binance_source.last_error() << "\n";
    }
  }

  return 0;
}

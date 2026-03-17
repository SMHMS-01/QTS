#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

#include "backtest/deterministic_clock.hpp"
#include "backtest/replay_engine.hpp"
#include "core/bus/event_bus.hpp"
#include "market/feed/tardis_incremental_book_l2_csv.hpp"
#include "market/normalization/sequence_guard.hpp"
#include "market/normalization/symbol_map.hpp"
#include "market/order_book/order_book.hpp"

namespace {

struct BacktestResult {
  int total_trades = 0;
  double total_pnl = 0.0;
  double sharpe_ratio = 0.0;
  double max_drawdown = 0.0;
  int winning_trades = 0;
  double win_rate = 0.0;
};

class OrderBookSink final : public core::bus::IEventSink {
public:
  explicit OrderBookSink(market::normalization::SequenceGuard* guard)
      : guard_(guard) {
    assert(guard_ != nullptr);
  }

  void on_event(const core::bus::Event& e) override {
    if (!guard_->accept(e.sequence)) {
      return;
    }
    const auto* msg = static_cast<const market::feed::DepthUpdate*>(e.payload);
    if (!msg) {
      return;
    }
    if (msg->is_snapshot && msg->header.exchange_time.ts.value != last_snapshot_ts_) {
      book_.clear();
      last_snapshot_ts_ = msg->header.exchange_time.ts.value;
    }
    market::order_book::UpdateContext ctx{msg->header.symbol, e.sequence};
    book_.apply_update(msg->is_bid ? market::order_book::Side::Bid
                                   : market::order_book::Side::Ask,
                       msg->price,
                       msg->quantity,
                       &ctx);
  }

  market::order_book::OrderBookL2& book() { return book_; }

private:
  market::normalization::SequenceGuard* guard_ = nullptr;
  market::order_book::OrderBookL2 book_;
  std::int64_t last_snapshot_ts_ = -1;
};

double compute_sharpe(const std::vector<double>& returns) {
  if (returns.size() < 2) {
    return 0.0;
  }
  double mean = 0.0;
  for (const auto r : returns) {
    mean += r;
  }
  mean /= static_cast<double>(returns.size());
  double variance = 0.0;
  for (const auto r : returns) {
    const double diff = r - mean;
    variance += diff * diff;
  }
  variance /= static_cast<double>(returns.size() - 1);
  const double stddev = std::sqrt(variance);
  if (stddev == 0.0) {
    return 0.0;
  }
  return mean / stddev * std::sqrt(static_cast<double>(returns.size()));
}

} // namespace

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: imbalance_backtest_demo <tardis_csv_path>\n";
    return 1;
  }

  const std::string path = argv[1];
  market::normalization::SimpleStringSymbolMap symbol_map;
  market::normalization::IdentityTimeAligner time_aligner;
  market::feed::TardisIncrementalBookL2CsvReplay source(path, &symbol_map, &time_aligner);

  backtest::DeterministicClock clock({});
  core::bus::EventBus bus;
  backtest::ReplayEngine engine(&clock, &source, &bus);

  market::normalization::SequenceGuard guard;
  OrderBookSink book_sink(&guard);
  bus.subscribe(core::bus::EventType::MarketData, &book_sink);

  int steps = 0;
  int position = 0;
  double cash = 0.0;
  double peak_equity = 0.0;
  double max_drawdown = 0.0;
  int total_trades = 0;
  int winning_trades = 0;
  double last_realized = 0.0;
  std::vector<double> returns;
  double prev_equity = 0.0;
  int debug_count = 0;
  std::int64_t last_decision_ts = 0;
  std::int64_t last_trade_ts = 0;
  int confirm_count = 0;

  const int depth_levels = 5;
  const double price_scale =
      static_cast<double>(market::feed::TardisIncrementalBookL2CsvReplay::kPriceScale);
  const double fee_rate = 0.0005;
  const int debug_steps = 20;
  const std::int64_t decision_interval_ns = 100'000'000LL; // 100ms
  const std::int64_t min_hold_ns = 500'000'000LL; // 500ms
  const int confirm_required = 3;
  while (engine.step()) {
    ++steps;

    market::order_book::Level bid{};
    market::order_book::Level ask{};
    if (!book_sink.book().best_bid(bid) || !book_sink.book().best_ask(ask)) {
      continue;
    }

    const auto now = engine.current_time().ts.value;
    if (now - last_decision_ts < decision_interval_ns) {
      continue;
    }
    last_decision_ts = now;

    const auto bid_depth = book_sink.book().depth_bid(depth_levels);
    const auto ask_depth = book_sink.book().depth_ask(depth_levels);
    const auto denom = static_cast<double>(bid_depth + ask_depth);
    if (denom == 0.0) {
      continue;
    }
    const double signal = (static_cast<double>(bid_depth - ask_depth)) / denom;

    int target = 0;
    if (signal > 0.3) {
      ++confirm_count;
      if (confirm_count >= confirm_required) {
        target = 1;
      }
    } else if (signal < -0.3) {
      --confirm_count;
      if (confirm_count <= -confirm_required) {
        target = -1;
      }
    } else {
      confirm_count = 0;
    }

    if (position != 0 && (now - last_trade_ts) < min_hold_ns) {
      target = position;
    }

    const double mid = ((static_cast<double>(bid.price.value) +
                         static_cast<double>(ask.price.value)) /
                        2.0) /
                       price_scale;
    const double equity = cash + position * mid;
    if (steps > 1) {
      returns.push_back(equity - prev_equity);
    }
    prev_equity = equity;

    if (equity > peak_equity) {
      peak_equity = equity;
    }
    const double drawdown = peak_equity - equity;
    if (drawdown > max_drawdown) {
      max_drawdown = drawdown;
    }

    if (debug_count < debug_steps) {
      std::cout << "step=" << steps
                << " signal=" << signal
                << " target=" << target
                << " bid=" << (static_cast<double>(bid.price.value) / price_scale)
                << " ask=" << (static_cast<double>(ask.price.value) / price_scale)
                << " bid_depth=" << bid_depth
                << " ask_depth=" << ask_depth
                << "\n";
      ++debug_count;
    }

    const int trade_qty = target - position;
    if (trade_qty != 0) {
      const double price = trade_qty > 0 ? static_cast<double>(ask.price.value) / price_scale
                                         : static_cast<double>(bid.price.value) / price_scale;
      cash -= price * static_cast<double>(trade_qty);
      cash -= price * static_cast<double>(std::abs(trade_qty)) * fee_rate;
      position += trade_qty;
      last_trade_ts = now;
      total_trades += std::abs(trade_qty);

      if (position == 0) {
        const double trade_pnl = cash - last_realized;
        if (trade_pnl > 0.0) {
          ++winning_trades;
        }
        last_realized = cash;
      }
    }
  }

  BacktestResult result{};
  result.total_trades = total_trades;
  result.total_pnl = prev_equity;
  result.sharpe_ratio = compute_sharpe(returns);
  result.max_drawdown = max_drawdown;
  result.winning_trades = winning_trades;
  result.win_rate = total_trades > 0
                        ? static_cast<double>(winning_trades) /
                              static_cast<double>(total_trades)
                        : 0.0;

  std::cout << "Steps=" << steps << "\n";
  std::cout << "TotalTrades=" << result.total_trades << "\n";
  std::cout << "TotalPnL=" << result.total_pnl << "\n";
  std::cout << "Sharpe=" << result.sharpe_ratio << "\n";
  std::cout << "MaxDrawdown=" << result.max_drawdown << "\n";
  std::cout << "WinningTrades=" << result.winning_trades << "\n";
  std::cout << "WinRate=" << result.win_rate << "\n";

  return 0;
}

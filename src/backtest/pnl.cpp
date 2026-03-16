#include "backtest/pnl.hpp"

namespace backtest {

class PnLTracker final : public IPnLTracker {
public:
  void on_fill(core::types::SymbolId, core::types::QuantityLots qty,
               core::types::PriceTicks price) override {
    position_ += qty.value;
    cash_ -= static_cast<double>(qty.value) * price.value;
  }

  void mark_to_market(core::types::SymbolId, core::types::PriceTicks price) override {
    last_price_ = price.value;
  }

  PnL snapshot() const override {
    PnL out;
    out.unrealized = cash_ + position_ * last_price_;
    out.realized = 0.0;
    out.fees = 0.0;
    return out;
  }

  Metrics metrics() const override { return {}; }

private:
  int position_ = 0;
  double cash_ = 0.0;
  double last_price_ = 0.0;
};

} // namespace backtest

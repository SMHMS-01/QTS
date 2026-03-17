#include "backtest/pnl.hpp"

#include "observability/metrics.hpp"

namespace backtest {

PnLTracker::PnLTracker(observability::IMetrics* metrics) : metrics_(metrics) {}

void PnLTracker::set_metrics(observability::IMetrics* metrics) {
  metrics_ = metrics;
}

void PnLTracker::on_fill(core::types::SymbolId, core::types::QuantityLots qty,
                         core::types::PriceTicks price) {
  position_ += qty.value;
  cash_ -= static_cast<double>(qty.value) * price.value;
  emit_metrics();
}

void PnLTracker::mark_to_market(core::types::SymbolId, core::types::PriceTicks price) {
  last_price_ = static_cast<double>(price.value);
  emit_metrics();
}

PnL PnLTracker::snapshot() const {
  PnL out;
  out.unrealized = cash_ + position_ * last_price_;
  out.realized = 0.0;
  out.fees = 0.0;
  out.position = static_cast<double>(position_);
  out.notional = position_ * last_price_;
  return out;
}

Metrics PnLTracker::metrics() const { return {}; }

void PnLTracker::emit_metrics() const {
  if (!metrics_) {
    return;
  }
  const auto current = snapshot();
  metrics_->gauge("pnl.realized").set(current.realized);
  metrics_->gauge("pnl.position").set(current.position);
  metrics_->gauge("pnl.notional").set(current.notional);
}

} // namespace backtest

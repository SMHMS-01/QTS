#pragma once

#include "core/types/types.hpp"

namespace observability {
class IMetrics;
}

namespace backtest {

struct PnL {
  double realized = 0.0;
  double unrealized = 0.0;
  double fees = 0.0;
  double position = 0.0;
  double notional = 0.0;
};

struct Metrics {
  double drawdown = 0.0;
  double sharpe = 0.0;
};

class IPnLTracker {
public:
  virtual ~IPnLTracker() = default;
  virtual void on_fill(core::types::SymbolId symbol,
                       core::types::QuantityLots qty,
                       core::types::PriceTicks price) = 0;
  virtual void mark_to_market(core::types::SymbolId symbol,
                              core::types::PriceTicks price) = 0;
  virtual PnL snapshot() const = 0;
  virtual Metrics metrics() const = 0;
};

class PnLTracker final : public IPnLTracker {
public:
  explicit PnLTracker(::observability::IMetrics* metrics = nullptr);
  void set_metrics(::observability::IMetrics* metrics);

  void on_fill(core::types::SymbolId symbol,
               core::types::QuantityLots qty,
               core::types::PriceTicks price) override;
  void mark_to_market(core::types::SymbolId symbol, core::types::PriceTicks price) override;
  PnL snapshot() const override;
  Metrics metrics() const override;

private:
  void emit_metrics() const;

  ::observability::IMetrics* metrics_ = nullptr;
  int position_ = 0;
  double cash_ = 0.0;
  double last_price_ = 0.0;
};

} // namespace backtest

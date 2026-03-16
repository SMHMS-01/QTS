#pragma once

#include "core/types/types.hpp"

namespace backtest {

struct PnL {
  double realized = 0.0;
  double unrealized = 0.0;
  double fees = 0.0;
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

} // namespace backtest

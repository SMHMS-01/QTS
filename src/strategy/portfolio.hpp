#pragma once

#include "core/types/types.hpp"

namespace strategy {

struct Position {
  core::types::SymbolId symbol;
  core::types::QuantityLots quantity;
  core::types::PriceTicks avg_price;
};

class Portfolio {
public:
  virtual ~Portfolio() = default;
  virtual void apply_trade(core::types::SymbolId symbol,
                           core::types::QuantityLots qty,
                           core::types::PriceTicks price) = 0;
  virtual Position position(core::types::SymbolId symbol) const = 0;
  virtual double pnl() const = 0;
};

} // namespace strategy

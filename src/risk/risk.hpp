#pragma once

#include <string>

#include "core/types/types.hpp"

namespace risk {

struct RiskCheckResult {
  bool ok = true;
  std::string reason;
};

struct OrderRequest {
  core::types::SymbolId symbol;
  core::types::QuantityLots quantity;
  core::types::PriceTicks price;
};

struct TradeFill {
  core::types::SymbolId symbol;
  core::types::QuantityLots quantity;
  core::types::PriceTicks price;
};

class IRiskEngine {
public:
  virtual ~IRiskEngine() = default;
  virtual RiskCheckResult check_order(const OrderRequest& order) = 0;
  virtual void on_fill(const TradeFill& fill) = 0;
};

} // namespace risk

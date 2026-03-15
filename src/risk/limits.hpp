#pragma once

#include <cstdint>

#include "core/types/types.hpp"

namespace risk {

struct Exposure {
  core::types::SymbolId symbol;
  core::types::QuantityLots net_qty;
};

struct Limit {
  core::types::SymbolId symbol;
  core::types::QuantityLots max_abs_qty;
};

struct LimitStatus {
  bool ok = true;
};

class IRiskLimits {
public:
  virtual ~IRiskLimits() = default;
  virtual void update_on_fill(core::types::SymbolId symbol,
                              core::types::QuantityLots qty,
                              core::types::PriceTicks price) = 0;
  virtual LimitStatus status(core::types::SymbolId symbol) const = 0;
};

} // namespace risk

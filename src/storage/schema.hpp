#pragma once

#include "core/time/time.hpp"
#include "core/types/types.hpp"

namespace storage {

struct MarketDataRow {
  core::types::SymbolId symbol;
  core::time::TimePoint exchange_time;
  core::time::TimePoint receive_time;
};

struct OrderRow {
  core::types::OrderId order_id;
  core::types::SymbolId symbol;
  core::time::TimePoint time;
};

struct TradeRow {
  core::types::TradeId trade_id;
  core::types::SymbolId symbol;
  core::types::PriceTicks price;
  core::types::QuantityLots quantity;
  core::time::TimePoint time;
};

struct SnapshotRow {
  core::types::SymbolId symbol;
  std::uint64_t sequence = 0;
  core::time::TimePoint time;
};

} // namespace storage

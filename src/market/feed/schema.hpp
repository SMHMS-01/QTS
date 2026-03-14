#pragma once

#include <cstdint>

#include "core/time/time.hpp"
#include "core/types/types.hpp"

namespace market::feed {

enum class FeedMessageType : std::uint8_t {
  BookSnapshot,
  BookUpdate,
  Trade
};

struct FeedHeader {
  core::types::SymbolId symbol;
  std::uint64_t sequence = 0;
  core::time::TimePoint exchange_time;
  core::time::TimePoint receive_time;
};

struct BookLevel {
  core::types::PriceTicks price;
  core::types::QuantityLots quantity;
};

struct BookSnapshot {
  FeedHeader header;
  const BookLevel* bids = nullptr;
  std::uint32_t bid_count = 0;
  const BookLevel* asks = nullptr;
  std::uint32_t ask_count = 0;
};

struct BookUpdate {
  FeedHeader header;
  bool is_bid = false;
  core::types::PriceTicks price;
  core::types::QuantityLots quantity;
};

struct Trade {
  FeedHeader header;
  core::types::PriceTicks price;
  core::types::QuantityLots quantity;
  bool is_buyer_maker = false;
};

} // namespace market::feed

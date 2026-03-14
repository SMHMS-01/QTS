#pragma once

#include "market/feed/schema.hpp"

namespace market::normalization {

struct NormalizedBookUpdate {
  market::feed::FeedHeader header;
  bool is_bid = false;
  core::types::PriceTicks price;
  core::types::QuantityLots quantity;
};

struct NormalizedTrade {
  market::feed::FeedHeader header;
  core::types::PriceTicks price;
  core::types::QuantityLots quantity;
  bool is_buyer_maker = false;
};

class Normalizer {
public:
  virtual ~Normalizer() = default;
  virtual NormalizedBookUpdate normalize_book_update(const market::feed::BookUpdate& msg) = 0;
  virtual NormalizedTrade normalize_trade(const market::feed::Trade& msg) = 0;
};

} // namespace market::normalization

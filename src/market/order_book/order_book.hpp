#pragma once

#include <map>

#include "core/types/types.hpp"

namespace market::order_book {

enum class Side : std::uint8_t {
  Bid,
  Ask
};

struct Level {
  core::types::PriceTicks price;
  core::types::QuantityLots quantity;
};

class OrderBookL2 {
public:
  void clear();
  void apply_update(Side side, core::types::PriceTicks price, core::types::QuantityLots quantity);

  bool best_bid(Level& out) const;
  bool best_ask(Level& out) const;

private:
  struct BidCompare {
    bool operator()(core::types::PriceTicks a, core::types::PriceTicks b) const {
      return a.value > b.value;
    }
  };

  std::map<core::types::PriceTicks, core::types::QuantityLots, BidCompare> bids_;
  std::map<core::types::PriceTicks, core::types::QuantityLots> asks_;
};

} // namespace market::order_book

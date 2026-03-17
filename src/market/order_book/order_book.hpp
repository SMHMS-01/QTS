#pragma once

#include <cstdint>
#include <map>

#include "core/types/types.hpp"
#include "observability/logging.hpp"

namespace market::order_book {

enum class Side : std::uint8_t {
  Bid,
  Ask
};

struct Level {
  core::types::PriceTicks price;
  core::types::QuantityLots quantity;
};

struct UpdateContext {
  core::types::SymbolId symbol;
  std::uint64_t sequence = 0;
};

class OrderBookL2 {
public:
  void set_logger(observability::ILogger* logger);

  void clear();
  void apply_update(Side side, core::types::PriceTicks price, core::types::QuantityLots quantity);
  void apply_update(Side side, core::types::PriceTicks price, core::types::QuantityLots quantity,
                    const UpdateContext* ctx);

  bool best_bid(Level& out) const;
  bool best_ask(Level& out) const;
  std::int64_t depth_bid(int levels) const;
  std::int64_t depth_ask(int levels) const;
  double imbalance() const;

private:
  struct BidCompare {
    bool operator()(core::types::PriceTicks a, core::types::PriceTicks b) const {
      return a.value > b.value;
    }
  };

  std::map<core::types::PriceTicks, core::types::QuantityLots, BidCompare> bids_;
  std::map<core::types::PriceTicks, core::types::QuantityLots> asks_;
  observability::ILogger* logger_ = nullptr;
};

} // namespace market::order_book

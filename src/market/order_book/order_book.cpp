#include "market/order_book/order_book.hpp"

namespace market::order_book {

void OrderBookL2::clear() {
  bids_.clear();
  asks_.clear();
}

void OrderBookL2::apply_update(Side side, core::types::PriceTicks price,
                               core::types::QuantityLots quantity) {
  if (side == Side::Bid) {
    if (quantity.value == 0) {
      bids_.erase(price);
    } else {
      bids_[price] = quantity;
    }
  } else {
    if (quantity.value == 0) {
      asks_.erase(price);
    } else {
      asks_[price] = quantity;
    }
  }
}

bool OrderBookL2::best_bid(Level& out) const {
  if (bids_.empty()) {
    return false;
  }
  const auto& it = bids_.begin();
  out.price = it->first;
  out.quantity = it->second;
  return true;
}

bool OrderBookL2::best_ask(Level& out) const {
  if (asks_.empty()) {
    return false;
  }
  const auto& it = asks_.begin();
  out.price = it->first;
  out.quantity = it->second;
  return true;
}

} // namespace market::order_book

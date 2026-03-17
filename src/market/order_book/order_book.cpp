#include "market/order_book/order_book.hpp"

#include <string>

namespace market::order_book {

namespace {

const char* side_label(Side side) {
  return side == Side::Bid ? "Bid" : "Ask";
}

} // namespace

void OrderBookL2::set_logger(observability::ILogger* logger) {
  logger_ = logger;
}

void OrderBookL2::clear() {
  bids_.clear();
  asks_.clear();
}

void OrderBookL2::apply_update(Side side, core::types::PriceTicks price,
                               core::types::QuantityLots quantity) {
  apply_update(side, price, quantity, nullptr);
}

void OrderBookL2::apply_update(Side side, core::types::PriceTicks price,
                               core::types::QuantityLots quantity, const UpdateContext* ctx) {
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

  if (logger_) {
    const auto symbol = ctx ? ctx->symbol.value : 0U;
    const auto sequence = ctx ? ctx->sequence : 0U;
    std::string message = "order_book.update symbol=" + std::to_string(symbol) +
                          " side=" + side_label(side) +
                          " price=" + std::to_string(price.value) +
                          " qty=" + std::to_string(quantity.value) +
                          " sequence=" + std::to_string(sequence);
    logger_->info(message);
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

std::int64_t OrderBookL2::depth_bid(int levels) const {
  if (levels <= 0) {
    return 0;
  }
  std::int64_t total = 0;
  int count = 0;
  for (const auto& [price, qty] : bids_) {
    (void)price;
    total += qty.value;
    if (++count >= levels) {
      break;
    }
  }
  return total;
}

std::int64_t OrderBookL2::depth_ask(int levels) const {
  if (levels <= 0) {
    return 0;
  }
  std::int64_t total = 0;
  int count = 0;
  for (const auto& [price, qty] : asks_) {
    (void)price;
    total += qty.value;
    if (++count >= levels) {
      break;
    }
  }
  return total;
}

double OrderBookL2::imbalance() const {
  const auto bid = static_cast<double>(depth_bid(1));
  const auto ask = static_cast<double>(depth_ask(1));
  const auto denom = bid + ask;
  if (denom == 0.0) {
    return 0.0;
  }
  return (bid - ask) / denom;
}

} // namespace market::order_book

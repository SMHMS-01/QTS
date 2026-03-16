#include <iostream>

#include "market/order_book/order_book.hpp"

int main() {
  using market::order_book::OrderBookL2;
  using market::order_book::Level;
  using market::order_book::Side;

  OrderBookL2 book;
  book.apply_update(Side::Bid, {100}, {10});
  book.apply_update(Side::Ask, {101}, {7});

  Level bid{};
  Level ask{};

  const bool has_bid = book.best_bid(bid);
  const bool has_ask = book.best_ask(ask);

  std::cout << "OrderBook L2 Demo\n";
  if (has_bid) {
    std::cout << "Best Bid: " << bid.price.value << " x " << bid.quantity.value << "\n";
  } else {
    std::cout << "Best Bid: (empty)\n";
  }
  if (has_ask) {
    std::cout << "Best Ask: " << ask.price.value << " x " << ask.quantity.value << "\n";
  } else {
    std::cout << "Best Ask: (empty)\n";
  }

  return 0;
}

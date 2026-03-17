#include <cassert>

#include "market/order_book/order_book.hpp"

using market::order_book::OrderBookL2;
using market::order_book::Level;
using market::order_book::Side;

int main() {
  OrderBookL2 book;
  Level level{};

  book.apply_update(Side::Bid, {100}, {10});
  book.apply_update(Side::Bid, {99}, {5});
  book.apply_update(Side::Ask, {101}, {7});
  book.apply_update(Side::Ask, {102}, {3});

  assert(book.best_bid(level));
  assert(level.price.value == 100);
  assert(level.quantity.value == 10);

  assert(book.best_ask(level));
  assert(level.price.value == 101);
  assert(level.quantity.value == 7);

  book.apply_update(Side::Bid, {100}, {0});
  book.apply_update(Side::Ask, {101}, {0});
  book.apply_update(Side::Bid, {98}, {4});

  assert(book.best_bid(level));
  assert(level.price.value == 99);
  assert(level.quantity.value == 5);

  assert(book.best_ask(level));
  assert(level.price.value == 102);
  assert(level.quantity.value == 3);

  book.clear();
  assert(!book.best_bid(level));
  assert(!book.best_ask(level));

  return 0;
}

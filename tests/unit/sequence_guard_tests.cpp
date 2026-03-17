#include <cassert>

#include "market/normalization/sequence_guard.hpp"

using market::normalization::SequenceGuard;

int main() {
  SequenceGuard guard;

  assert(guard.accept(1));
  assert(guard.accept(2));
  assert(!guard.accept(2)); // duplicate
  assert(!guard.accept(4)); // gap
  assert(guard.last() == 2);

  assert(guard.accept(3)); // fills the gap in order
  assert(!guard.accept(1)); // out-of-order
  assert(guard.last() == 3);

  return 0;
}

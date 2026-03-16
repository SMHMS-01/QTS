#include <cassert>

#include "execution/order_manager.hpp"

int main() {
  // compile-time sanity: types exist
  execution::OrderRequest req{};
  (void)req;
  return 0;
}

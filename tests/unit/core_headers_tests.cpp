#include "core/bus/event.hpp"
#include "core/threading/queue.hpp"
#include "core/threading/threading.hpp"
#include "core/time/time.hpp"
#include "core/types/types.hpp"

int main() {
  core::types::PriceTicks p{1};
  core::types::QuantityLots q{2};
  (void)p;
  (void)q;
  return 0;
}

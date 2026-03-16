#include <cassert>

#include "risk/risk.hpp"

int main() {
  risk::OrderRequest req{};
  req.quantity.value = 1;
  risk::RiskCheckResult res{true, ""};
  assert(res.ok);
  return 0;
}

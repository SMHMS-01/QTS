#include "risk/risk.hpp"

namespace risk {

class RiskEngine final : public IRiskEngine {
public:
  explicit RiskEngine(core::types::QuantityLots max_qty) : max_qty_(max_qty) {}

  RiskCheckResult check_order(const OrderRequest& order) override {
    if (order.quantity.value <= 0) {
      return {false, "invalid quantity"};
    }
    if (order.quantity.value > max_qty_.value) {
      return {false, "quantity exceeds limit"};
    }
    return {true, ""};
  }

  void on_fill(const TradeFill& fill) override {
    (void)fill;
  }

private:
  core::types::QuantityLots max_qty_{};
};

} // namespace risk

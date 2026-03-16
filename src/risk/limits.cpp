#include "risk/limits.hpp"

#include <unordered_map>

namespace risk {

class RiskLimits final : public IRiskLimits {
public:
  explicit RiskLimits(core::types::QuantityLots max_abs) : max_abs_(max_abs) {}

  void update_on_fill(core::types::SymbolId symbol,
                      core::types::QuantityLots qty,
                      core::types::PriceTicks) override {
    auto& current = net_[symbol.value];
    current.value += qty.value;
  }

  LimitStatus status(core::types::SymbolId symbol) const override {
    const auto it = net_.find(symbol.value);
    if (it == net_.end()) {
      return {true};
    }
    const auto abs = it->second.value < 0 ? -it->second.value : it->second.value;
    return {abs <= max_abs_.value};
  }

private:
  core::types::QuantityLots max_abs_{};
  std::unordered_map<std::uint32_t, core::types::QuantityLots> net_;
};

} // namespace risk

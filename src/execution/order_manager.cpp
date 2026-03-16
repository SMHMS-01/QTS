#include "execution/order_manager.hpp"

#include <unordered_map>

namespace execution {

namespace {

struct OrderState {
  OrderStatus status = OrderStatus::New;
  core::types::QuantityLots remaining;
};

} // namespace

class OrderManager final : public IOrderManager {
public:
  OrderAck submit(const OrderRequest& order) override {
    const core::types::OrderId id{next_id_++};
    OrderState state{};
    state.status = OrderStatus::New;
    state.remaining = order.quantity;
    orders_.emplace(id.value, state);
    return {id, OrderStatus::New};
  }

  void cancel(core::types::OrderId id) override {
    auto it = orders_.find(id.value);
    if (it != orders_.end()) {
      it->second.status = OrderStatus::Cancelled;
    }
  }

  void on_fill(const OrderFill& fill) override {
    auto it = orders_.find(fill.id.value);
    if (it == orders_.end()) {
      return;
    }

    if (fill.quantity.value >= it->second.remaining.value) {
      it->second.remaining.value = 0;
      it->second.status = OrderStatus::Filled;
    } else {
      it->second.remaining.value -= fill.quantity.value;
      it->second.status = OrderStatus::PartiallyFilled;
    }
  }

private:
  std::uint64_t next_id_ = 1;
  std::unordered_map<std::uint64_t, OrderState> orders_;
};

} // namespace execution

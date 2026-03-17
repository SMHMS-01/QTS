#include "execution/order_manager.hpp"

namespace execution {

OrderAck OrderManager::submit(const OrderRequest& order) {
  const core::types::OrderId id{next_id_++};
  OrderState state{};
  state.status = OrderStatus::New;
  state.remaining = order.quantity;
  orders_.emplace(id.value, state);
  return {id, OrderStatus::New};
}

void OrderManager::cancel(core::types::OrderId id) {
  auto it = orders_.find(id.value);
  if (it != orders_.end()) {
    it->second.status = OrderStatus::Cancelled;
  }
}

void OrderManager::on_fill(const OrderFill& fill) {
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

} // namespace execution

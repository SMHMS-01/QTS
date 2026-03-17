#include "execution/broker_adaptor.hpp"

namespace execution {

bool SimBrokerAdaptor::connect() {
  connected_ = true;
  return true;
}

void SimBrokerAdaptor::disconnect() {
  connected_ = false;
}

OrderAck SimBrokerAdaptor::send_order(const OrderRequest& order) {
  if (!connected_ || !manager_ || !book_) {
    return {core::types::OrderId{0}, OrderStatus::Rejected};
  }

  auto ack = manager_->submit(order);

  market::order_book::Level level{};
  bool has_price = false;
  if (order.side == OrderSide::Buy) {
    has_price = book_->best_ask(level);
  } else {
    has_price = book_->best_bid(level);
  }

  if (!has_price) {
    return {ack.id, OrderStatus::Rejected};
  }

  OrderFill fill{ack.id, order.quantity, level.price};
  manager_->on_fill(fill);
  return {ack.id, OrderStatus::Filled};
}

void SimBrokerAdaptor::cancel(core::types::OrderId id) {
  if (manager_) {
    manager_->cancel(id);
  }
}

void SimBrokerAdaptor::on_execution_report(const ExecutionReport&) {}

} // namespace execution

#pragma once

#include "execution/order_manager.hpp"

namespace execution {

struct ExecutionReport {
  core::types::OrderId id;
  OrderStatus status = OrderStatus::New;
  core::types::QuantityLots filled_qty;
  core::types::PriceTicks avg_price;
};

class IBrokerAdaptor {
public:
  virtual ~IBrokerAdaptor() = default;
  virtual bool connect() = 0;
  virtual void disconnect() = 0;
  virtual OrderAck send_order(const OrderRequest& order) = 0;
  virtual void cancel(core::types::OrderId id) = 0;
  virtual void on_execution_report(const ExecutionReport& report) = 0;
};

} // namespace execution

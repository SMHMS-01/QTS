#pragma once

#include "execution/order_manager.hpp"
#include "market/order_book/order_book.hpp"

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

class SimBrokerAdaptor final : public IBrokerAdaptor {
public:
  SimBrokerAdaptor(market::order_book::OrderBookL2* book, IOrderManager* manager)
      : book_(book), manager_(manager) {}

  bool connect() override;
  void disconnect() override;
  OrderAck send_order(const OrderRequest& order) override;
  void cancel(core::types::OrderId id) override;
  void on_execution_report(const ExecutionReport& report) override;

private:
  market::order_book::OrderBookL2* book_ = nullptr;
  IOrderManager* manager_ = nullptr;
  bool connected_ = false;
};

} // namespace execution

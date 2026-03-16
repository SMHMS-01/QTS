#include "execution/broker_adaptor.hpp"

namespace execution {

class BrokerAdaptor final : public IBrokerAdaptor {
public:
  bool connect() override {
    connected_ = true;
    return true;
  }

  void disconnect() override { connected_ = false; }

  OrderAck send_order(const OrderRequest& order) override {
    (void)order;
    return {core::types::OrderId{0}, OrderStatus::New};
  }

  void cancel(core::types::OrderId) override {}

  void on_execution_report(const ExecutionReport&) override {}

private:
  bool connected_ = false;
};

} // namespace execution

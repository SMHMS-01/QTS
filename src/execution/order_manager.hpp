#pragma once

#include <cstdint>
#include <unordered_map>

#include "core/types/types.hpp"

namespace execution {

enum class OrderSide : std::uint8_t {
  Buy,
  Sell
};

enum class OrderType : std::uint8_t {
  Limit,
  Market
};

enum class OrderStatus : std::uint8_t {
  New,
  PartiallyFilled,
  Filled,
  Cancelled,
  Rejected
};

struct OrderRequest {
  core::types::SymbolId symbol;
  OrderSide side = OrderSide::Buy;
  OrderType type = OrderType::Limit;
  core::types::QuantityLots quantity;
  core::types::PriceTicks price;
};

struct OrderAck {
  core::types::OrderId id;
  OrderStatus status = OrderStatus::New;
};

struct OrderFill {
  core::types::OrderId id;
  core::types::QuantityLots quantity;
  core::types::PriceTicks price;
};

class IOrderManager {
public:
  virtual ~IOrderManager() = default;
  virtual OrderAck submit(const OrderRequest& order) = 0;
  virtual void cancel(core::types::OrderId id) = 0;
  virtual void on_fill(const OrderFill& fill) = 0;
};

class OrderManager final : public IOrderManager {
public:
  OrderAck submit(const OrderRequest& order) override;
  void cancel(core::types::OrderId id) override;
  void on_fill(const OrderFill& fill) override;

private:
  struct OrderState {
    OrderStatus status = OrderStatus::New;
    core::types::QuantityLots remaining;
  };

  std::uint64_t next_id_ = 1;
  std::unordered_map<std::uint64_t, OrderState> orders_;
};

} // namespace execution

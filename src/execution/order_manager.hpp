#pragma once

#include <cstdint>

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

} // namespace execution

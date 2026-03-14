#pragma once

#include <cstdint>

namespace core::types {

struct TimestampNs {
  std::int64_t value = 0; // nanoseconds since epoch or session start
};

struct PriceTicks {
  std::int64_t value = 0; // price in ticks
};

struct QuantityLots {
  std::int64_t value = 0; // quantity in lots or units
};

struct OrderId {
  std::uint64_t value = 0;
};

struct TradeId {
  std::uint64_t value = 0;
};

struct SymbolId {
  std::uint32_t value = 0;
};

inline constexpr bool operator==(TimestampNs a, TimestampNs b) { return a.value == b.value; }
inline constexpr bool operator==(PriceTicks a, PriceTicks b) { return a.value == b.value; }
inline constexpr bool operator==(QuantityLots a, QuantityLots b) { return a.value == b.value; }
inline constexpr bool operator==(OrderId a, OrderId b) { return a.value == b.value; }
inline constexpr bool operator==(TradeId a, TradeId b) { return a.value == b.value; }
inline constexpr bool operator==(SymbolId a, SymbolId b) { return a.value == b.value; }

inline constexpr bool operator<(TimestampNs a, TimestampNs b) { return a.value < b.value; }
inline constexpr bool operator<(PriceTicks a, PriceTicks b) { return a.value < b.value; }
inline constexpr bool operator<(QuantityLots a, QuantityLots b) { return a.value < b.value; }
inline constexpr bool operator<(OrderId a, OrderId b) { return a.value < b.value; }
inline constexpr bool operator<(TradeId a, TradeId b) { return a.value < b.value; }
inline constexpr bool operator<(SymbolId a, SymbolId b) { return a.value < b.value; }

inline constexpr PriceTicks operator+(PriceTicks a, PriceTicks b) { return PriceTicks{a.value + b.value}; }
inline constexpr PriceTicks operator-(PriceTicks a, PriceTicks b) { return PriceTicks{a.value - b.value}; }
inline constexpr QuantityLots operator+(QuantityLots a, QuantityLots b) { return QuantityLots{a.value + b.value}; }
inline constexpr QuantityLots operator-(QuantityLots a, QuantityLots b) { return QuantityLots{a.value - b.value}; }

} // namespace core::types

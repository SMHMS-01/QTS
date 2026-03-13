#pragma once

#include <cstdint>

namespace core::types {

struct Timestamp {
  std::int64_t value = 0; // nanoseconds since epoch or session start
};

struct Price {
  std::int64_t value = 0; // price in ticks
};

struct Quantity {
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

inline constexpr bool operator==(Timestamp a, Timestamp b) { return a.value == b.value; }
inline constexpr bool operator==(Price a, Price b) { return a.value == b.value; }
inline constexpr bool operator==(Quantity a, Quantity b) { return a.value == b.value; }
inline constexpr bool operator==(OrderId a, OrderId b) { return a.value == b.value; }
inline constexpr bool operator==(TradeId a, TradeId b) { return a.value == b.value; }
inline constexpr bool operator==(SymbolId a, SymbolId b) { return a.value == b.value; }

inline constexpr bool operator<(Timestamp a, Timestamp b) { return a.value < b.value; }
inline constexpr bool operator<(Price a, Price b) { return a.value < b.value; }
inline constexpr bool operator<(Quantity a, Quantity b) { return a.value < b.value; }
inline constexpr bool operator<(OrderId a, OrderId b) { return a.value < b.value; }
inline constexpr bool operator<(TradeId a, TradeId b) { return a.value < b.value; }
inline constexpr bool operator<(SymbolId a, SymbolId b) { return a.value < b.value; }

inline constexpr Price operator+(Price a, Price b) { return Price{a.value + b.value}; }
inline constexpr Price operator-(Price a, Price b) { return Price{a.value - b.value}; }
inline constexpr Quantity operator+(Quantity a, Quantity b) { return Quantity{a.value + b.value}; }
inline constexpr Quantity operator-(Quantity a, Quantity b) { return Quantity{a.value - b.value}; }

} // namespace core::types

#pragma once

#include "market/feed/schema.hpp"

namespace market::normalization {

struct NormalizedBookUpdate {
  market::feed::FeedHeader header;
  bool is_bid = false;
  core::types::PriceTicks price;
  core::types::QuantityLots quantity;
};

struct NormalizedTrade {
  market::feed::FeedHeader header;
  core::types::PriceTicks price;
  core::types::QuantityLots quantity;
  bool is_buyer_maker = false;
};

class TimeAligner {
public:
  virtual ~TimeAligner() = default;
  virtual core::time::TimePoint align(core::time::TimePoint exchange_time) = 0;
};

class IdentityTimeAligner final : public TimeAligner {
public:
  core::time::TimePoint align(core::time::TimePoint exchange_time) override { return exchange_time; }
};

class OffsetTimeAligner final : public TimeAligner {
public:
  explicit OffsetTimeAligner(std::int64_t offset_ns) : offset_ns_(offset_ns) {}

  core::time::TimePoint align(core::time::TimePoint exchange_time) override {
    exchange_time.ts.value += offset_ns_;
    exchange_time.type = core::time::ClockType::Receive;
    return exchange_time;
  }

private:
  std::int64_t offset_ns_ = 0;
};

class Normalizer {
public:
  virtual ~Normalizer() = default;
  virtual NormalizedBookUpdate normalize_book_update(const market::feed::BookUpdate& msg) = 0;
  virtual NormalizedTrade normalize_trade(const market::feed::Trade& msg) = 0;
};

} // namespace market::normalization

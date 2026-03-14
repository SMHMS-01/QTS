#pragma once

#include <cstddef>

#include "market/feed/schema.hpp"

namespace market::feed {

class ReplaySource {
public:
  virtual ~ReplaySource() = default;
  virtual const FeedHeader* peek_header() const = 0;
  virtual bool next() = 0;
  virtual const void* payload() const = 0;
  virtual FeedMessageType payload_type() const = 0;
};

struct ReplayConfig {
  core::time::TimePoint start_time;
  core::time::TimePoint end_time;
  double speed = 1.0;
  std::size_t max_events = 0;
};

} // namespace market::feed

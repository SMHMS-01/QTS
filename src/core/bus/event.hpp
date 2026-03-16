#pragma once

#include <cstdint>

#include "core/time/time.hpp"

namespace core::bus {

enum class EventType : std::uint8_t {
  MarketData,
  Order,
  Trade,
  Timer,
  Control
};

struct Event {
  core::time::TimePoint time;
  EventType type = EventType::Control;
  std::uint64_t sequence = 0;
  void* payload = nullptr;
};

class IEventSink {
public:
  virtual ~IEventSink() = default;
  virtual void on_event(const Event& event) = 0;
};

} // namespace core::bus

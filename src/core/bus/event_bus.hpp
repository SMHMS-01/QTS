#pragma once

#include <array>

#include "core/bus/event.hpp"

namespace core::bus {

class EventBus {
public:
  void subscribe(EventType type, IEventSink* sink);
  void publish(const Event& event) const;

private:
  static constexpr std::size_t kMaxSinksPerType = 16;
  using SinkList = std::array<IEventSink*, kMaxSinksPerType>;

  std::array<SinkList, static_cast<std::size_t>(EventType::Control) + 1> sinks_{};
  std::array<std::size_t, static_cast<std::size_t>(EventType::Control) + 1> counts_{};
};

} // namespace core::bus

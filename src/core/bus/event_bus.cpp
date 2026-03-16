#include "core/bus/event_bus.hpp"

namespace core::bus {

void EventBus::subscribe(EventType type, IEventSink* sink) {
  const auto idx = static_cast<std::size_t>(type);
  if (idx >= sinks_.size() || counts_[idx] >= kMaxSinksPerType) {
    return;
  }
  sinks_[idx][counts_[idx]] = sink;
  counts_[idx] += 1;
}

void EventBus::publish(const Event& event) const {
  const auto idx = static_cast<std::size_t>(event.type);
  if (idx >= sinks_.size()) {
    return;
  }
  const auto count = counts_[idx];
  for (std::size_t i = 0; i < count; ++i) {
    if (auto* sink = sinks_[idx][i]) {
      sink->on_event(event);
    }
  }
}

} // namespace core::bus

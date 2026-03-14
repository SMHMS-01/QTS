#pragma once

#include <cstdint>

namespace core::threading {

enum class ThreadingModel : std::uint8_t {
  SingleThread,
  ProducerConsumer,
  WorkStealing
};

} // namespace core::threading

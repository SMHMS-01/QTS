#pragma once

#include <cstdint>

namespace market::normalization {

class SequenceGuard {
public:
  bool accept(std::uint64_t seq) {
    if (seq <= last_) {
      return false;
    }
    last_ = seq;
    return true;
  }

  std::uint64_t last() const { return last_; }

private:
  std::uint64_t last_ = 0;
};

} // namespace market::normalization

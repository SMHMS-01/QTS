#pragma once

#include <cstddef>

namespace core::threading {

template <typename T>
class IQueue {
public:
  virtual ~IQueue() = default;
  virtual bool push(const T& value) = 0;
  virtual bool try_pop(T& out) = 0;
  virtual std::size_t size() const = 0;
};

template <typename T>
class SpscQueue final : public IQueue<T> {
public:
  bool push(const T& value) override = 0;
  bool try_pop(T& out) override = 0;
  std::size_t size() const override = 0;
};

} // namespace core::threading

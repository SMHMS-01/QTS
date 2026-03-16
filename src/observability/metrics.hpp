#pragma once

#include <string>

namespace observability {

class Counter {
public:
  virtual ~Counter() = default;
  virtual void inc(double value = 1.0) = 0;
};

class Gauge {
public:
  virtual ~Gauge() = default;
  virtual void set(double value) = 0;
};

class Histogram {
public:
  virtual ~Histogram() = default;
  virtual void observe(double value) = 0;
};

class IMetrics {
public:
  virtual ~IMetrics() = default;
  virtual Counter& counter(const std::string& name) = 0;
  virtual Gauge& gauge(const std::string& name) = 0;
  virtual Histogram& histogram(const std::string& name) = 0;
};

} // namespace observability

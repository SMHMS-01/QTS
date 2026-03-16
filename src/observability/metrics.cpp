#include "observability/metrics.hpp"

#include <unordered_map>

namespace observability {

namespace {

class CounterImpl final : public Counter {
public:
  void inc(double value = 1.0) override { value_ += value; }
  double value() const { return value_; }

private:
  double value_ = 0.0;
};

class GaugeImpl final : public Gauge {
public:
  void set(double value) override { value_ = value; }
  double value() const { return value_; }

private:
  double value_ = 0.0;
};

class HistogramImpl final : public Histogram {
public:
  void observe(double value) override { last_ = value; }
  double last() const { return last_; }

private:
  double last_ = 0.0;
};

} // namespace

class MetricsRegistry final : public IMetrics {
public:
  Counter& counter(const std::string& name) override { return counters_[name]; }
  Gauge& gauge(const std::string& name) override { return gauges_[name]; }
  Histogram& histogram(const std::string& name) override { return histograms_[name]; }

private:
  std::unordered_map<std::string, CounterImpl> counters_;
  std::unordered_map<std::string, GaugeImpl> gauges_;
  std::unordered_map<std::string, HistogramImpl> histograms_;
};

} // namespace observability

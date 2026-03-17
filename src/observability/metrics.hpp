#pragma once

#include <string>
#include <unordered_map>

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

class CounterImpl final : public Counter {
public:
  CounterImpl() = default;
  explicit CounterImpl(std::string name);
  void inc(double value = 1.0) override;
  double value() const { return value_; }

private:
  std::string name_;
  double value_ = 0.0;
};

class GaugeImpl final : public Gauge {
public:
  GaugeImpl() = default;
  explicit GaugeImpl(std::string name);
  void set(double value) override;
  double value() const { return value_; }

private:
  std::string name_;
  double value_ = 0.0;
};

class HistogramImpl final : public Histogram {
public:
  HistogramImpl() = default;
  explicit HistogramImpl(std::string name);
  void observe(double value) override;
  double last() const { return last_; }

private:
  std::string name_;
  double last_ = 0.0;
};

class MetricsRegistry final : public IMetrics {
public:
  Counter& counter(const std::string& name) override;
  Gauge& gauge(const std::string& name) override;
  Histogram& histogram(const std::string& name) override;

private:
  std::unordered_map<std::string, CounterImpl> counters_;
  std::unordered_map<std::string, GaugeImpl> gauges_;
  std::unordered_map<std::string, HistogramImpl> histograms_;
};

class StdoutMetrics final : public IMetrics {
public:
  Counter& counter(const std::string& name) override;
  Gauge& gauge(const std::string& name) override;
  Histogram& histogram(const std::string& name) override;

private:
  std::unordered_map<std::string, CounterImpl> counters_;
  std::unordered_map<std::string, GaugeImpl> gauges_;
  std::unordered_map<std::string, HistogramImpl> histograms_;
};

} // namespace observability

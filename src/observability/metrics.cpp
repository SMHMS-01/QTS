#include "observability/metrics.hpp"

#include <iostream>

namespace observability {

CounterImpl::CounterImpl(std::string name) : name_(std::move(name)) {}

void CounterImpl::inc(double value) {
  value_ += value;
  if (!name_.empty()) {
    std::cout << "[METRIC] " << name_ << "=" << value_ << "\n";
  }
}

GaugeImpl::GaugeImpl(std::string name) : name_(std::move(name)) {}

void GaugeImpl::set(double value) {
  value_ = value;
  if (!name_.empty()) {
    std::cout << "[METRIC] " << name_ << "=" << value_ << "\n";
  }
}

HistogramImpl::HistogramImpl(std::string name) : name_(std::move(name)) {}

void HistogramImpl::observe(double value) {
  last_ = value;
  if (!name_.empty()) {
    std::cout << "[METRIC] " << name_ << "=" << last_ << "\n";
  }
}

Counter& MetricsRegistry::counter(const std::string& name) {
  return counters_[name];
}

Gauge& MetricsRegistry::gauge(const std::string& name) {
  return gauges_[name];
}

Histogram& MetricsRegistry::histogram(const std::string& name) {
  return histograms_[name];
}

Counter& StdoutMetrics::counter(const std::string& name) {
  auto it = counters_.find(name);
  if (it == counters_.end()) {
    it = counters_.emplace(name, CounterImpl{name}).first;
  }
  return it->second;
}

Gauge& StdoutMetrics::gauge(const std::string& name) {
  auto it = gauges_.find(name);
  if (it == gauges_.end()) {
    it = gauges_.emplace(name, GaugeImpl{name}).first;
  }
  return it->second;
}

Histogram& StdoutMetrics::histogram(const std::string& name) {
  auto it = histograms_.find(name);
  if (it == histograms_.end()) {
    it = histograms_.emplace(name, HistogramImpl{name}).first;
  }
  return it->second;
}

} // namespace observability

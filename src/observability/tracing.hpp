#pragma once

#include <string>

namespace observability {

struct Span {
  std::string name;
};

class ITracer {
public:
  virtual ~ITracer() = default;
  virtual Span start_span(const std::string& name) = 0;
  virtual void end_span(const Span& span) = 0;
  virtual void annotate(const Span& span, const std::string& key, const std::string& value) = 0;
};

class TracerImpl final : public ITracer {
public:
  Span start_span(const std::string& name) override;
  void end_span(const Span& span) override;
  void annotate(const Span& span, const std::string& key, const std::string& value) override;
};

} // namespace observability

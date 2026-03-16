#include "observability/tracing.hpp"

namespace observability {

class TracerImpl final : public ITracer {
public:
  Span start_span(const std::string& name) override { return Span{name}; }
  void end_span(const Span&) override {}
  void annotate(const Span&, const std::string&, const std::string&) override {}
};

} // namespace observability

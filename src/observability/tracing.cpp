#include "observability/tracing.hpp"

namespace observability {

Span TracerImpl::start_span(const std::string& name) {
  return Span{name};
}

void TracerImpl::end_span(const Span&) {}

void TracerImpl::annotate(const Span&, const std::string&, const std::string&) {}

} // namespace observability

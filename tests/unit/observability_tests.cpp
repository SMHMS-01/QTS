#include "observability/logging.hpp"
#include "observability/metrics.hpp"
#include "observability/tracing.hpp"

int main() {
  observability::Span span{"root"};
  (void)span;
  return 0;
}

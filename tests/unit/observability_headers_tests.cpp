#include "observability/logging.hpp"
#include "observability/metrics.hpp"
#include "observability/tracing.hpp"

int main() {
  observability::Span s{"test"};
  (void)s;
  return 0;
}

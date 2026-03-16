#include "observability/logging.hpp"

#include <iostream>

namespace observability {

class StdoutLogger final : public ILogger {
public:
  void info(const std::string& message) override {
    std::cout << "[INFO] " << message << "\n";
  }

  void warn(const std::string& message) override {
    std::cout << "[WARN] " << message << "\n";
  }

  void error(const std::string& message) override {
    std::cerr << "[ERROR] " << message << "\n";
  }
};

} // namespace observability

#include "observability/logging.hpp"

#include <iostream>

namespace observability {

void StdoutLogger::info(const std::string& message) {
  std::cout << "[INFO] " << message << "\n";
}

void StdoutLogger::warn(const std::string& message) {
  std::cout << "[WARN] " << message << "\n";
}

void StdoutLogger::error(const std::string& message) {
  std::cerr << "[ERROR] " << message << "\n";
}

} // namespace observability

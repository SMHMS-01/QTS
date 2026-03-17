#pragma once

#include <string>

namespace observability {

class ILogger {
public:
  virtual ~ILogger() = default;
  virtual void info(const std::string& message) = 0;
  virtual void warn(const std::string& message) = 0;
  virtual void error(const std::string& message) = 0;
};

class StdoutLogger final : public ILogger {
public:
  void info(const std::string& message) override;
  void warn(const std::string& message) override;
  void error(const std::string& message) override;
};

} // namespace observability

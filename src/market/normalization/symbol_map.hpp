#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

#include "core/types/types.hpp"

namespace market::normalization {

class SymbolMap {
public:
  virtual ~SymbolMap() = default;
  virtual core::types::SymbolId map(std::uint64_t external) const = 0;
};

class StringSymbolMap {
public:
  virtual ~StringSymbolMap() = default;
  virtual core::types::SymbolId map(std::string_view external) = 0;
};

class SimpleStringSymbolMap final : public StringSymbolMap {
public:
  core::types::SymbolId map(std::string_view external) override;

private:
  std::unordered_map<std::string, core::types::SymbolId> symbols_;
  std::uint32_t next_id_ = 1;
};

} // namespace market::normalization

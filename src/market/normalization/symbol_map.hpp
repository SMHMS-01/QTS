#pragma once

#include <cstdint>

#include "core/types/types.hpp"

namespace market::normalization {

class SymbolMap {
public:
  virtual ~SymbolMap() = default;
  virtual core::types::SymbolId map(std::uint64_t external) const = 0;
};

} // namespace market::normalization

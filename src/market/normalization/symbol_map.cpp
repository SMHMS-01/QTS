#include "market/normalization/symbol_map.hpp"

namespace market::normalization {

core::types::SymbolId SimpleStringSymbolMap::map(std::string_view external) {
  if (external.empty()) {
    return core::types::SymbolId{0};
  }
  auto it = symbols_.find(std::string(external));
  if (it == symbols_.end()) {
    const core::types::SymbolId id{next_id_++};
    it = symbols_.emplace(std::string(external), id).first;
  }
  return it->second;
}

} // namespace market::normalization

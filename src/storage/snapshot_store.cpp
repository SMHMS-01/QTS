#include "storage/snapshot_store.hpp"

namespace storage {

void MemorySnapshotStore::save(core::types::SymbolId symbol, const SnapshotRow& snapshot) {
  data_[symbol.value] = snapshot;
}

SnapshotRow MemorySnapshotStore::load(core::types::SymbolId symbol) {
  const auto it = data_.find(symbol.value);
  if (it == data_.end()) {
    return SnapshotRow{};
  }
  return it->second;
}

} // namespace storage

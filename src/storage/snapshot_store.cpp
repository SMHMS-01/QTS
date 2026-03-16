#include "storage/snapshot_store.hpp"

#include <unordered_map>

namespace storage {

class MemorySnapshotStore final : public ISnapshotStore {
public:
  void save(core::types::SymbolId symbol, const SnapshotRow& snapshot) override {
    data_[symbol.value] = snapshot;
  }

  SnapshotRow load(core::types::SymbolId symbol) override {
    const auto it = data_.find(symbol.value);
    if (it == data_.end()) {
      return SnapshotRow{};
    }
    return it->second;
  }

private:
  std::unordered_map<std::uint32_t, SnapshotRow> data_;
};

} // namespace storage

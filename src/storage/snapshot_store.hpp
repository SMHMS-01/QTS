#pragma once

#include <unordered_map>

#include "core/types/types.hpp"
#include "storage/schema.hpp"

namespace storage {

class ISnapshotStore {
public:
  virtual ~ISnapshotStore() = default;
  virtual void save(core::types::SymbolId symbol, const SnapshotRow& snapshot) = 0;
  virtual SnapshotRow load(core::types::SymbolId symbol) = 0;
};

class MemorySnapshotStore final : public ISnapshotStore {
public:
  void save(core::types::SymbolId symbol, const SnapshotRow& snapshot) override;
  SnapshotRow load(core::types::SymbolId symbol) override;

private:
  std::unordered_map<std::uint32_t, SnapshotRow> data_;
};

} // namespace storage

#pragma once

#include "core/types/types.hpp"
#include "storage/schema.hpp"

namespace storage {

class ISnapshotStore {
public:
  virtual ~ISnapshotStore() = default;
  virtual void save(core::types::SymbolId symbol, const SnapshotRow& snapshot) = 0;
  virtual SnapshotRow load(core::types::SymbolId symbol) = 0;
};

} // namespace storage

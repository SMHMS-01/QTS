#include <cassert>

#include "storage/snapshot_store.hpp"

int main() {
  storage::MemorySnapshotStore store;

  storage::SnapshotRow row{};
  row.symbol = {42};
  row.sequence = 7;
  row.time = {core::types::TimestampNs{123456}, core::time::ClockType::Exchange};

  store.save(row.symbol, row);
  const auto loaded = store.load(row.symbol);

  assert(loaded.symbol.value == 42);
  assert(loaded.sequence == 7);
  assert(loaded.time == row.time);

  const auto missing = store.load({100});
  assert(missing.sequence == 0);

  return 0;
}

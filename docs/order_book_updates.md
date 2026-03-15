# Order Book Incremental Updates (T3.3)

Version: 0.1
Date: 2026-03-15

## Overview
This document defines the normalized incremental update semantics for the order book. The goal is deterministic application with strict sequence enforcement.

## Update Model
Each update applies to a single symbol and carries a monotonic sequence id.

### Update Fields
- `side`: bid or ask
- `price`: price level (ticks)
- `quantity`: quantity (lots)
- `action`: add, modify, delete
- `sequence`: monotonic per symbol

## Semantics (L2)
- Add: insert level at price if not present, otherwise treat as modify.
- Modify: set level quantity to the provided value (not delta).
- Delete: remove the price level.

## Semantics (L3)
- Add: add a new order with price-time priority at its level.
- Modify: update quantity of existing order id.
- Delete: remove the order id.

## Sequence Rules
- If `sequence` <= last applied, reject and flag.
- If `sequence` jumps by more than 1, flag a gap and require snapshot recovery.

## Snapshot Interaction
- A snapshot replaces the full book state and resets sequence tracking.

## Error Handling
- Unknown order id on modify/delete (L3): reject and flag.
- Invalid side or negative quantity: reject and flag.

## Determinism Guarantees
- Deterministic ordering for equal prices using time priority.
- No unordered traversal in update application.

# Order Book Snapshot Load/Save (T3.4)

Version: 0.1
Date: 2026-03-15

## Overview
This document defines the snapshot schema and the load/save semantics for order book state.

## Snapshot Schema

### L2 Snapshot
- `bids`: array of `{price, quantity}` sorted by price descending.
- `asks`: array of `{price, quantity}` sorted by price ascending.
- `sequence`: last applied sequence id.

### L3 Snapshot
- `bids`: array of orders grouped by price, each order has `{order_id, price, quantity, time}`.
- `asks`: array of orders grouped by price, each order has `{order_id, price, quantity, time}`.
- `sequence`: last applied sequence id.

## Load Semantics
- Snapshot replaces the entire book state.
- Sequence tracking is reset to snapshot `sequence`.
- Book invariants must hold after load.

## Save Semantics
- Save current book into schema above.
- Maintain deterministic ordering: price, then time priority within level.

## Error Handling
- Invalid ordering: reject snapshot and flag.
- Missing fields: reject snapshot and flag.

## Determinism
- For identical state, snapshot serialization must be stable and reproducible.

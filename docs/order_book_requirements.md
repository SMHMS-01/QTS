# Order Book Requirements (T3.1)

Version: 0.1
Date: 2026-03-15

## Scope
- Support L2 and L3 order book representations.
- Inputs: snapshot and incremental updates from normalized feed.
- Deterministic replay aligned with simulation clock.

## Book Types
- L2: aggregated by price level with total quantity.
- L3: individual orders with price-time priority.

## Inputs
- Snapshot: full book state at a sequence.
- Incremental update: add/modify/delete levels or orders.
- Trade: optional, used for validation and stats.

## Sequencing and Consistency
- Monotonic sequence IDs are required per symbol.
- Out-of-order updates must be rejected and flagged.
- Missing sequence detection should be exposed to caller.

## Time Model
- Each event carries exchange time and receive time.
- Replay uses simulation clock for determinism.

## Query API (Minimum)
- Top of book: best bid/ask price and size.
- Depth: N levels per side.
- Imbalance: configurable depth window.

## Performance Targets (Initial)
- Correctness and determinism are priority over speed.
- O(log N) updates for L2; L3 may be O(log N) with price-time ordering.

## Error Handling
- Reject invalid sequences.
- Allow explicit reset via snapshot.

## Testing
- Deterministic replay test with fixed input sequence.
- Invariant checks: price-time priority, sequence monotonicity.

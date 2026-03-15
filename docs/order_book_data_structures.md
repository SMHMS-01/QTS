# Order Book Data Structures and Complexity Goals (T3.2)

Version: 0.1
Date: 2026-03-15

## Overview
This document defines the target data structures for L2 and L3 books and their complexity goals. The design prioritizes determinism and correctness over raw throughput at this stage.

## L2 Book (Aggregated Levels)

Structure
- Two ordered maps per symbol: `bids` (desc price), `asks` (asc price).
- Each level stores `{price, total_quantity}`.

Operations and Complexity
- Insert/update/delete level: `O(log N)`.
- Top-of-book query: `O(1)` by caching best bid/ask iterators.
- Depth query: `O(K)` for K levels.

Notes
- Ordering via `std::map` or custom tree; direct array indexing is only possible with fixed tick grids.

## L3 Book (Order-Level)

Structure
- Price map: `price -> FIFO queue of orders`.
- Orders: `{order_id, price, quantity, side, time}`.

Operations and Complexity
- Add order: `O(log N)` for price level + `O(1)` enqueue.
- Modify order: `O(1)` if order pointer/index is known; else `O(log N)` lookup.
- Remove order: `O(1)` if linked list node; else `O(log N)` + `O(1)`.
- Best bid/ask: `O(1)` by cached best-level iterators.

## Snapshot Representation
- L2 snapshot: arrays of levels for bids and asks.
- L3 snapshot: arrays of orders grouped by price (or flat list with price grouping rules).

## Memory and Determinism
- Avoid unordered traversal in deterministic replay.
- Maintain stable ordering within same price for L3 (price-time priority).

## Alternatives Considered
- Hash map + heap for best price: reduces lookup but complicates determinism and updates.
- Fixed tick arrays: fastest, but requires bounded price range.

## Target Constraints (Initial)
- Determinism across replays with identical input.
- No undefined ordering of equal-price orders.

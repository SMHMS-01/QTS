# Order Book Query API (T3.5)

Version: 0.1
Date: 2026-03-15

## Overview
This document defines the minimum query interface for the order book. All queries are read-only and deterministic.

## Data Types
- `Level { price, quantity }`
- `TopOfBook { best_bid, best_ask }`

## APIs

### Top of Book
- Returns best bid and best ask levels.
- If one side is empty, return a null/empty level for that side.

### Depth
- `depth(side, K)` returns up to K levels for the given side.
- Levels are ordered by price priority.

### Imbalance
- `imbalance(K)` computes imbalance over top K levels:
  - `imbalance = (sum_bid_qty - sum_ask_qty) / (sum_bid_qty + sum_ask_qty)`
  - If denominator is zero, return 0.

## L2 vs L3
- L2 returns aggregated levels directly.
- L3 returns aggregated levels by default (sum of order quantities per price).

## Determinism
- Query results must be stable given identical book state.

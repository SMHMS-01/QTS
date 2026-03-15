# Order Book Deterministic Tests (T3.6)

Version: 0.1
Date: 2026-03-15

## Overview
This document defines deterministic test cases for the order book. Tests are specification-level and must be reproducible with identical input sequences.

## Test Inputs
- Snapshot + incremental updates with fixed sequence ids.
- Use a fixed symbol and deterministic timepoints.

## Core Tests

### T1: Snapshot Load
- Apply snapshot with bids/asks.
- Assert top-of-book matches expected.

### T2: Incremental Updates
- Apply add/modify/delete updates.
- Assert level quantities and ordering.

### T3: Sequence Rules
- Reject update with duplicate or lower sequence.
- Detect and flag gaps.

### T4: Query API
- Validate depth(K) ordering.
- Validate imbalance(K) formula with expected values.

### T5: Determinism
- Run identical input twice, compare outputs bit-for-bit.

## Edge Cases
- Empty book (no bids/asks).
- One-sided book.
- Delete non-existent level/order.

## Expected Outcomes
- No nondeterminism.
- Strict price-time priority maintained.

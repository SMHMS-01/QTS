# Architecture

Version: 0.1
Date: 2026-03-16

## Overview
This project is a modular C++23 quant trading system. The architecture is layered to separate data ingestion, market state, strategy, risk, execution, and backtest/simulation.

## Core Layers
- Ingestion: market data feeds and historical replay.
- Normalization: schema validation, symbol mapping, time normalization.
- Market State: order book and derived microstructure metrics.
- Strategy Engine: signal generation and portfolio logic.
- Risk Engine: pre-trade checks and post-trade limits.
- Execution: order manager and broker adaptor.
- Backtest & Simulation: deterministic clock and event-driven replay.
- Storage: schemas and persistence of data and snapshots.
- Observability: logging, metrics, tracing.

## Data Flow (High-Level)
1. Feed data -> normalization -> event bus.
2. Market state updates -> strategy signals -> risk checks.
3. Execution -> fills -> portfolio + risk updates.
4. Backtest uses deterministic clock and replay sources.

## Time Model
- Exchange time and receive time are preserved for events.
- Deterministic clock is used in simulation/backtest to ensure reproducibility.

## Threading Model
- Threading is explicitly defined; queues and event dispatch are the integration points.
- Initial implementation favors single-threaded determinism.

## Build and Test
- CMake + Ninja, C++23.
- Minimal compile tests for module headers and a basic L2 order book test.

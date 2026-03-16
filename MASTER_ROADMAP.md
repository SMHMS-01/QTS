# MASTER_ROADMAP (Prior Knowledge)

Version: 0.1
Date: 2026-03-13
Scope: Quant Trading System (Clang 19, C++23)

Purpose
- This file is the global architecture and dependency map for the project.
- Before any micro-task, read this file to ensure local changes do not break global consistency.

Completion Criteria
- [x] currently means the spec/interface is defined and accepted.
- Implementation status is tracked separately in MASTER_ROADMAP_IMPL.md.


1. Goals and Non-Goals

Goals
- Deterministic, low-latency order book and matching core with clear correctness guarantees.
- Modular architecture for data, strategy, risk, execution, and research.
- Reproducible backtesting aligned with live trading logic.
- Strong observability, testing, and simulation support.

Non-Goals (initial phase)
- High-frequency colocation optimizations beyond baseline latency hygiene.
- Multi-asset exotic derivatives pricing engines.
- Full GUI frontend.


2. Architecture Overview

Core runtime layers
- Ingestion: market data feeds and historical replay.
- Normalization: symbol mapping, timestamping, schema validation.
- Market State: order book, trades, quotes, microstructure stats.
- Strategy Engine: signal generation, portfolio logic.
- Risk Engine: pre-trade and post-trade risk checks.
- Execution: order routing, order management, broker/adaptor layer.
- Backtest & Simulation: event-driven replay, deterministic clock.
- Storage: time series, snapshots, configs.
- Observability: logging, metrics, tracing, profiling.

Key cross-cutting concerns
- Determinism in simulation and backtest.
- Threading model defined and enforced (lock strategy, queues, atomics).
- Time model (exchange time, receive time, local time).
- Error handling, retries, and fail-fast policy.


3. Technology Stack

Compiler and Language
- Clang 19
- C++23

Build and Tooling
- CMake (>= 3.28)
- Ninja
- vcpkg or Conan (decide once dependencies grow)
- clang-format, clang-tidy
- ccache

Testing and Benchmarking
- Catch2 or GoogleTest (choose one)
- Google Benchmark

Libraries (initial)
- fmt (formatting)
- spdlog (logging)
- nlohmann/json or simdjson (config)
- Boost (only if needed)

Observability
- OpenTelemetry (optional later)


4. Directory Structure (Target)

/ (repo root)
- MASTER_ROADMAP.md
- README.md
- CMakeLists.txt
- /cmake
- /docs
  - architecture.md
  - decisions.md
- /configs
- /data
  - /raw
  - /processed
- /scripts
- /third_party
- /src
  - /common
  - /core
    - time
    - types
    - bus
    - threading
  - /market
    - order_book
    - feed
    - normalization
  - /strategy
  - /risk
  - /execution
  - /backtest
  - /storage
  - /observability
- /tests
  - /unit
  - /integration
  - /perf
- /benchmarks
- /tools


5. Order Book Focus (Current Phase)

Order book scope
- L2/L3 order book data structures
- Snapshot and incremental updates
- Consistent sequencing and event time handling
- Book state queries (best bid/ask, depth, imbalance)
- Deterministic replay in simulation

Order book invariants
- Strict price-time priority
- Monotonic sequence ids for updates
- Consistent handling of out-of-order or missing updates


6. Task Dependency Tree (High-Level)

Legend
- [X] = completed
- [ ] = pending
- -> = dependency

T0: Repository bootstrap
- [x] T0.1: Create MASTER_ROADMAP.md (this file)
- [x] T0.2: Create README.md
- [x] T0.3: Setup CMake toolchain skeleton

T1: Core foundations
- [x] T1.1: Define core types (timestamps, ids, prices, quantities)
- [x] T1.2: Time model and clock abstraction
- [x] T1.3: Event bus / dispatcher
- [x] T1.4: Threading model and queues

T2: Market data ingestion
- [x] T2.1: Feed schema definitions
- [x] T2.2: Normalization layer
- [x] T2.3: Replay framework

T3: Order book (current focus)
- [x] T3.1: Order book requirements spec
- [x] T3.2: Data structures and complexity goals
- [x] T3.3: Apply incremental updates
- [x] T3.4: Snapshot load/save
- [x] T3.5: Query API (top of book, depth, imbalance)
- [x] T3.6: Deterministic tests

T4: Strategy engine
- [x] T4.1: Signal interface
- [x] T4.2: Portfolio model
- [x] T4.3: Strategy runner

T5: Risk engine
- [x] T5.1: Pre-trade checks
- [x] T5.2: Post-trade limits

T6: Execution
- [x] T6.1: Order manager
- [x] T6.2: Broker adaptor interface

T7: Backtest and simulation
- [x] T7.1: Deterministic clock
- [x] T7.2: Event-driven replay
- [x] T7.3: PnL and metrics

T8: Observability
- [x] T8.1: Logging
- [x] T8.2: Metrics
- [x] T8.3: Tracing

T9: Storage
- [x] T9.1: Data schema
- [x] T9.2: Snapshot persistence

Dependencies
- T1 -> T2, T3, T4, T5, T6, T7
- T2 -> T3, T7
- T3 -> T4, T7
- T5 -> T6
- T6 -> T7
- T8 -> all runtime components
- T9 -> T2, T3, T7


7. Decision Log (Initial)
- Compiler: Clang 19
- Language: C++23
- Build: CMake + Ninja
- Current focus: T3 Order Book


8. Update Policy
- Any new module or directory must be added here first.
- Any change in architecture, dependencies, or task order must update this file.

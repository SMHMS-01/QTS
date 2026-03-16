# Quant Trading System (Clang 19, C++23)

This repository is a C++23 quant trading system focused on deterministic order book modeling, event-driven simulation, and a modular path to live execution. The project is designed for correctness first, then performance.

## Scope (Current Phase)
- Order book core (L2/L3), snapshots, incremental updates
- Deterministic replay and backtest alignment
- Foundations for strategy, risk, and execution

## Architecture Snapshot
Runtime layers
- Ingestion
- Normalization
- Market State (order book)
- Strategy
- Risk
- Execution
- Backtest & Simulation
- Storage
- Observability

## Documentation
- MASTER_ROADMAP.md: global architecture and dependency map
- MASTER_ROADMAP_IMPL.md: implementation review ledger
- docs/architecture.md: architecture overview
- docs/decisions.md: key design decisions

## Tech Stack
- Compiler: Clang 19
- Language: C++23
- Build: CMake + Ninja
- Tooling: clang-format, clang-tidy, ccache
- Testing: Catch2 or GoogleTest (TBD), Google Benchmark

## Repository Layout (Target)
See MASTER_ROADMAP.md for the authoritative structure and dependency tree.

## Development Notes
- Determinism and correctness are primary constraints.
- Time model and threading model are defined before performance optimization.

## License
TBD

# Decisions

Version: 0.1
Date: 2026-03-16

## ADR-0001: Interface-First Development
- Decision: Define interfaces/specs before concrete implementations.
- Rationale: Stabilize architecture and dependencies early.
- Tradeoff: Implementation progress is slower initially.

## ADR-0002: Determinism as Primary Constraint
- Decision: Deterministic behavior for backtest and simulation is a hard requirement.
- Rationale: Enables reproducibility and reliable debugging.
- Tradeoff: Some performance optimizations are deferred.

## ADR-0003: Separate Implementation Ledger
- Decision: Track implementation status in MASTER_ROADMAP_IMPL.md.
- Rationale: Avoid conflating spec completion with implementation completion.
- Tradeoff: Two sources must be kept consistent.

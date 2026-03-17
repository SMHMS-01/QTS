# MASTER_ROADMAP_IMPL (Implementation Review)

Version: 0.2
Date: 2026-03-16

Purpose
- This file is the implementation review and approval ledger.
- It verifies whether roadmap items have concrete, buildable implementations.
- It complements MASTER_ROADMAP.md, which tracks architecture and specs.

Status Legend
- [X] Implemented and review-approved
- [ ] Not implemented
- [!] Implemented but not reviewed

Review Scope
- Check that each module has concrete code (not interface-only).
- Check that build targets exist (non-INTERFACE where appropriate).
- Check that at least one test exists for critical paths.

Findings (High-Level)
- Order book core has minimal L2 implementation; L3 and full update/query coverage are not complete.
- Backtest replay has minimal logic and passes basic tests; not feature-complete.
- Execution and risk have minimal in-memory implementations; behavior is not production-ready.
- Observability has minimal in-process implementations; no external backend integration.
- Storage has in-memory snapshot persistence only; no durable storage.
- Build system now has concrete targets; coverage is partial across modules.
- Architecture and decision docs exist but are not reviewed.

Findings (Details)
- Order book: `src/market/order_book/order_book.cpp` provides L2 only; no L3 or snapshot/query API implementation.
- Backtest: minimal replay plus demo executable (`backtest_demo`); no scheduling/filters.
- Execution: `src/execution/order_manager.cpp` and `src/execution/broker_adaptor.cpp` are minimal in-memory stubs.
- Risk: `src/risk/risk.cpp` and `src/risk/limits.cpp` are minimal checks and net limits.
- Observability: `src/observability/*.cpp` are minimal stdout/in-memory implementations.
- Storage: `src/storage/snapshot_store.cpp` is in-memory only.
- Build: concrete targets exist for core/backtest/execution/risk/observability/storage/order_book.
- Docs: `docs/architecture.md` and `docs/decisions.md` exist but are not reviewed.

Implementation Checklist
- [!] Order book implementation (L2/L3, updates, snapshot, query)
- [!] Backtest replay logic (deterministic clock integration)
- [!] Execution implementation (order manager, broker adaptor)
- [!] Risk implementation (pre-trade, post-trade)
- [!] Observability backend (logging, metrics, tracing)
- [!] Storage persistence backend
- [!] Concrete build targets (libraries/executables)
- [!] Minimal tests for core paths
- [!] Architecture and decision docs

Integration Priorities (from review feedback)
- [x] End-to-end minimal runnable path: replay -> normalization -> order_book -> strategy -> pnl output
- [x] Deterministic order_book correctness tests with known L2 snapshots + updates
- [x] sequence_guard tests for out-of-order and missing updates
- [x] snapshot_store save/load roundtrip validation
- [x] Runtime observability wiring: order_book structured logs + pnl metrics output
- [x] Replace dummy replay source with real historical dataset (e.g., depth snapshots + diffs)
- [!] SimBrokerAdaptor for paper trading (signal -> order -> fill -> position -> pnl)

Approval Notes
- Update this file when concrete implementations land and are reviewed.
- Use [!] when code exists but review is pending.

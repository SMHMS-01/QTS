---
session_id: S-2026-0313
task_id: T-049
status: PENDING
write_files:
  - ""
---

## Intent

## Constraints

## Feedback
对于e2e_demo.cpp文件
还有一个潜在问题值得注意
SequenceGuard::accept 接收的是 e.time.ts.value，但语义上它应该验证的是 sequence number，不是时间戳。两者在这个 demo 里恰好相等（都是 1–5），所以测试会通过，但这是巧合而非正确性。
如果你的 Event 结构里有独立的 sequence 字段（对应 FeedHeader::sequence），应该用那个：
cpp// 现在
if (!guard_->accept(e.time.ts.value)) { ... }

// 更准确（如果 Event 有 sequence 字段）
if (!guard_->accept(e.sequence)) { ... }
如果 Event 目前没有 sequence 字段，这是一个值得添加的字段——sequence 和时间是两个不同的保证：sequence 保证无 gap、不乱序；时间保证单调递增。两者都需要，但不能混用。

一个小结构建议
guard_ 是裸指针且公开，可以加一行保护：
cppexplicit OrderBookSink(market::normalization::SequenceGuard* guard) : guard_(guard) {
    assert(guard != nullptr);  // fail fast，而不是在 on_event 里每次判空
}

// on_event 里就可以去掉 if (guard_) 的判断，直接调用
现在每次 on_event 都 check if (guard_)，但构造时已经传入了非空指针，这个判断是防御多余的。要么构造时断言非空，要么允许 null 并在文档里说明——两种风格都行，但不要两者都做（既允许 null 又每次判空，语义模糊）。

这个 demo 现在能证明什么
验证点状态ReplayEngine 正确驱动 5 步✓EventBus 正确路由到 sink✓SequenceGuard 接入（序号连续时全部通过）✓（巧合正确，见上）OrderBook 正确处理 bid/ask update✓qty=0 正确撤单（100@0 从 book 移除）✓时钟推进接线正确✓
下一步自然是补一个负面测试：给 DummySource 注入一个 gap（比如 sequence 跳过 3），验证 SequenceGuard 确实会拦截，book 不会应用那条 update。这样才能证明 guard 真的在工作，而不只是在正常路径上被调用了。

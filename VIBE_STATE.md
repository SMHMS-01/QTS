---
session_id: S-2026-0313
task_id: T-050
status: PENDING
write_files:
  - ""
---

## Intent
read feedback

## Constraints

## Feedback
代码整体很扎实，负面测试路径也跑通了。逐点分析：

## 正确的地方

**sequence gap 测试设计是对的。** `{1, 2, 4, 5, 6}` 跳过了 3，后续的 4/5/6 取决于 `SequenceGuard` 的行为——这正是需要测试的边界条件。

**断言结果也随之正确更新了。** gap 之后的 msg[2]（Bid 99@5）被拦截，所以最终 best bid 仍然是 msg[0] 的 100@10，断言改成 `bid.price == 100, qty == 10` 是对的。

**`guard_` 构造时断言非空，`on_event` 里去掉了重复判断。** 语义干净了。

---

## 一个需要澄清的问题

当前测试隐含了一个关于 `SequenceGuard` 行为的假设，但没有显式说明：

sequences 是 `{1, 2, 4, 5, 6}`，gap 在 3。**gap 之后的消息（4、5、6）应该怎么处理？**

两种合理的实现：

```
策略 A（严格）：gap 之后全部拒绝，等待 resync
  accept(1) = true
  accept(2) = true
  accept(4) = false  ← gap
  accept(5) = false  ← gap 之后继续拒绝
  accept(6) = false
  → book 只有 msg[0] + msg[1]，best bid = 100@10 ✓

策略 B（宽松）：只拒绝 gap 本身，之后重新对齐
  accept(1) = true
  accept(2) = true
  accept(4) = false  ← gap（丢弃）
  accept(5) = true   ← 重新对齐到 5
  accept(6) = true
  → book 有 msg[0] + msg[1] + msg[3] + msg[4]
  → msg[4] 是 Bid 100@0，撤掉 100@10
  → best bid = 99@5（如果 msg[2] 没被应用）... 但 msg[2] 确实没应用
  → best bid = ??? 取决于 book 里还剩什么
```

你的断言 `bid.price == 100` 只在**策略 A** 下成立。如果 `SequenceGuard` 实现的是策略 B，断言会失败——而测试本身不会告诉你为什么。

**建议加一行注释说明假设：**

```cpp
// SequenceGuard 行为：gap 后所有消息拒绝，直到显式 reset()
// sequences {1,2,_,4,5,6}: msg[0]+msg[1] 应用，msg[2..4] 全部拒绝
// 预期 book: Bid=100@10, Ask=101@7
```

这样测试就是自文档化的，任何人读到断言值都知道它从哪里来。

---

## 一个小的结构问题

`msgs_` 有 5 个元素，`sequences_` 也有 5 个，但 `total_` 是运行时传入的。如果调用方写 `DummySource(3)`，`sequences_[3]` 和 `msgs_[3]` 不会被用到，但 `total_` 和数组大小的对齐完全靠调用方自觉：

```cpp
// 这样写不会编译报错，但 sequences_[5] 和 msgs_[5] 不一致
DummySource source(7);  // UB：payload() 会越界
```

可以加一个静态约束，或者直接去掉 `total_` 参数，用数组大小推导：

```cpp
static constexpr int kTotal = 5;
explicit DummySource() : total_(kTotal) {}
// 或者
bool next() override {
    if (index_ >= static_cast<int>(std::size(msgs_))) return false;
    ...
}
```

---

## 总结

| 点 | 状态 |
|---|---|
| payload 真正流动 | ✓ |
| SequenceGuard 接入且正确用 sequence 而非时间 | ✓ |
| 负面测试（gap 拦截）| ✓ 行为正确，假设未文档化 |
| 断言值与数据一致 | ✓ |
| DummySource 的 total 与数组大小解耦 | 待加固 |

现在这个 demo 真正在测试系统行为了，不只是跑通流程。

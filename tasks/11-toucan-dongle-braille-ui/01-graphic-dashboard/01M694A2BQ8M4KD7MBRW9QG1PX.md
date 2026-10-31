---
created: Sun 01 Nov 2026 - 01h 02m 40s
author:
  type: agent
  harness: codex
  model: gpt-6.1-sol
  name: coordinator
subject: Timed scrolling regression fixed and published
cargo:
  type: task
  status: delivered
---

1. Final timed validation caught repeated LVGL long-mode assignment restarting scrolling every 50 ms.
2. Toucan commit 2e89948 guards unchanged font and scrolling mode assignments.
3. A regression test failed before the fix and passes afterward for all twelve fixtures, exercising horizontal preview/buffer scrolling and vertical editor scrolling over repeated refreshes.
4. This follow-up supersedes the initial scrolling implementation in bd6b723.

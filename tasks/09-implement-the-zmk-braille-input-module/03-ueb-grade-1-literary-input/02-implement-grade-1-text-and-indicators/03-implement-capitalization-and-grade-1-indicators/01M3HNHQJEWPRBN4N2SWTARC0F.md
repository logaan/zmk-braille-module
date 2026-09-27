---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Implement capitalization and grade 1 indicators
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Implement capitalization and grade 1 indicators

1. Dot 6 before a lowercase letter capitalizes that letter.
2. Follow the rulebook for capitalized words and passages and for grade 1 indicators; indicators are context, never printable keys.
3. Unit-test alongside the implementation: dot-6 letter capitalization, capitalized words and passages, grade 1 indicators, no indicator emitted as a printable key, and an indicator followed by a chord that cannot continue it.

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

1. Handle dot-6 letter capitalization plus ICEB capitalized words/passages and grade 1 indicators without treating indicators as printable keys.
2. Unit-test capitalization and indicators alongside the implementation: dot-6 letter capitalization, capitalized words and passages, grade 1 indicators, and no indicator emitted as a printable key.

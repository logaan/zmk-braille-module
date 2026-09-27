---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Implement UEB numbers and numeric mode
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Implement UEB numbers and numeric mode

1. Use dots 3456 as numeric indicator; a–j give 1–0 across a digit run with rulebook-consistent entry and termination.
2. Unit-test numeric handling alongside the implementation: the dots 3456 indicator, `a`–`j` as 1–0, digit runs without a repeated indicator, and rulebook-consistent entry and termination.

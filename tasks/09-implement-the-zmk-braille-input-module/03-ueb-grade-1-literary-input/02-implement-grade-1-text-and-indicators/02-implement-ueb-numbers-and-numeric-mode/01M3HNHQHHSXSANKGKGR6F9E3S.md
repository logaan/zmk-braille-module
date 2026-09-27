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

1. Dots 3, 4, 5, 6 are the numeric indicator; following it, the patterns for `a` through `j` produce digits `1` through `0`.
2. The indicator applies across a run of digits under the rulebook's numeric-mode rules and is not repeated before every digit.
3. Follow the rulebook's numeric-mode entry, continuation, and termination.
4. Unit-test numeric handling alongside the implementation: the indicator, `a`–`j` as `1`–`0`, digit runs without a repeated indicator, and entry and termination.

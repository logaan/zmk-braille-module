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

1. Implement numeric mode as specified in `references/research/ueb-numeric-mode.yaml`, using the `numeric` context field of `references/research/ueb-indicator-context.yaml`.
2. The twelve numeric indicators are dots 3, 4, 5, 6 followed by `a`–`j` (digits `1`–`0`), by dot 2 (decimal comma), or by dots 2, 5, 6 (decimal point); each outputs its character and sets numeric mode.
3. In numeric mode, each of these keeps numeric mode:
    1. `a`–`j`: digits `1`–`0`.
    2. Dot 2 and dots 2, 5, 6: comma and full stop.
    3. Dot 5 followed by `a`–`j`: a space, then the digit.
4. A space or any other sign ends numeric mode and is read with numeric mode off; later digits need a new numeric indicator.
5. Dots 3, 4, 5, 6 followed by a chord that forms no numeric indicator is an incomplete prefix under the shared pending-input policy.
6. Add no rules for the spaced numeric indicator, numeric passage, line continuation, visible space, or simple fraction line.
7. Unit-test numeric handling alongside the implementation: each numeric indicator, digit runs without a repeated indicator, comma, full stop, and numeric space inside a number, termination by space, letters, capitals, the grade 1 indicator, and punctuation, and an orphaned numeric prefix.

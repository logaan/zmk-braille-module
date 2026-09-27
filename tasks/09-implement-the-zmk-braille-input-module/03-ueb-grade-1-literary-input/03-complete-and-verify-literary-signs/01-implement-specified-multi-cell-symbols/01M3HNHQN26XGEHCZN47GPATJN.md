---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Implement specified multi-cell symbols
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Implement specified multi-cell symbols

1. Cover every sign in the spec §5.3 table: ampersand through backslash, including delimiters and symbols, with ordered cells.
2. Unit-test every specified multi-cell symbol alongside the implementation, asserting ordered cells and correct host output.

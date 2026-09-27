---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Implement the computer8 character table
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Implement the computer8 character table

1. Add `braille_rules_computer8.c` with one cell per printable non-space ASCII character, using the common behavior-action representation.
2. Literal ASCII space comes from the Braille-space-only rule; the zero-dot pattern is reserved.
3. Keep every mapped and reserved entry consistent with the published table and free of conflicts with `computer8`'s command table.
4. Reserved entries send nothing to the host and raise the display notice.
5. Unit-test the table alongside the implementation: every eight-bit pattern is mapped or reserved including zero dots, all 94 printable non-space ASCII characters have one-cell input, reserved patterns produce no output and a notice, and backtick, vertical bar, case, shifted symbols, and independent dots 7 and 8 behave as documented.

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
2. Cover literal ASCII space through the shared space-only rule and account for the selected map's zero-dot entry.
3. Keep all mapped/reserved dot entries consistent with the published table and free of conflicts with always-active factory actions.
4. Unit-test the table alongside the implementation: every eight-bit dot pattern is explicitly mapped or reserved including zero dots, all 94 printable non-space ASCII characters have one-cell input, and backtick, vertical bar, case, shifted symbols, and independent dots 7/8 behave as documented.

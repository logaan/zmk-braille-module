---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: How should physical roles map to arbitrary key positions?
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# How should physical roles map to arbitrary key positions?

1. Determine how the parameterless behavior and configured positions receive dot/space events and the dedicated Braille-key gestures.
2. Keep `#binding-cells = <0>` and the nine configured dot/space roles from spec §3.2; do not turn the dedicated key into a tenth chord bit.
3. Account for conventional keymap behavior when Braille input is off, unaffected non-role keys, and ownership of press/release events during transitions.

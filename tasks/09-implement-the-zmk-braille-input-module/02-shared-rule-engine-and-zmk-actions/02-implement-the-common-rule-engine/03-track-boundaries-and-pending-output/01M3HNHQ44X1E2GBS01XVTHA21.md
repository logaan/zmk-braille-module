---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Track boundaries and pending output
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Track boundaries and pending output

1. Implement word-position constraints, the UEB context of `references/research/ueb-indicator-context.yaml`, and the pending-input policy in the common engine.
    1. After a rule commits, apply its class's row of the context transition table, or the capitals mode it sets.
    2. Ambiguous input stays pending until the next chord extends it or rules it out; there are no timers.
    3. In the initial modes, pending input never exceeds 8 cells, the rule-length bound; take the bound per mode so phase 07 can raise it to 32 cells for `ueb2`.
2. Discard pending input and reset UEB context to its initial values (numeric off, no capitals mode, word start set), without host output, on any layer change, non-Braille key press, character-mode change, command action, or settings entry; observe layer and non-Braille key events through ZMK event listeners.
3. Expose pending cells, their commit and discard lifecycle, and notices for dropped prefixes to phase 05's display.
4. Unit-test context and pending output alongside the implementation: each class's context transition, all word positions, ambiguous and unmatched prefixes, the 8-cell bound, and each transition leaving no stale cells or context.

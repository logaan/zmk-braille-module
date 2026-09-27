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

1. Implement word-position constraints, the resolved UEB context, and the pending-input policy in the common engine.
    1. Ambiguous input stays pending until the next chord extends it or rules it out; there are no timers.
    2. Pending input never exceeds 8 cells, the rule-length bound.
2. Discard pending input and reset UEB context, without host output, on any layer change, non-Braille key press, character-mode change, command action, or settings entry; observe layer and non-Braille key events through ZMK event listeners.
3. Expose pending cells, their commit and discard lifecycle, and notices for dropped prefixes to phase 05's display.
4. Unit-test context and pending output alongside the implementation: all word positions, ambiguous and unmatched prefixes, the 8-cell bound, and each transition leaving no stale cells or context.

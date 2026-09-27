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

1. Implement word-position constraints and the agreed UEB context, and the next-chord-decides commit policy in the common engine; there are no timers, and pending input is bounded by the 8-cell rule length.
2. Discard pending input and reset UEB context, without host output, on any layer change, non-Braille key press, character-mode change, command action, or settings entry; observe layer and non-Braille key events through ZMK event listeners.
3. Expose pending cells and their commit/cancel lifecycle for phase 05's display; do not defer all delayed-input feedback to Grade 2.
4. Unit-test context and pending output alongside the implementation: all word positions, ambiguous and unmatched prefixes, the 8-cell bound, and transition contracts that leave no stale cells or context.

---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Implement Braille-key toggle and settings gestures
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Implement Braille-key toggle and settings gestures

1. Short press toggles Braille input on/off; it does not cycle `ueb1`/`computer8` character tables.
2. Long press requests settings entry/exit without also firing the short-press toggle or entering the chord mask.
3. Implement transition/capture hooks and output suppression here; phase 05 connects the actual settings interface to these hooks.
4. Unit-test the gestures alongside the implementation: short/long exclusivity at the resolved threshold, exclusion from chord masks, and no unintended output across enable/disable and settings transitions, exercising hooks rather than the later UI.

---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Implement Braille-key toggle and settings gestures
cargo:
  type: task
  status: draft
---

# Implement Braille-key toggle and settings gestures

1. Short press toggles Braille input on/off; it does not cycle `ueb1`/`computer8` character tables.
2. Long press requests settings entry/exit without also firing the short-press toggle or entering the chord mask.
3. Implement transition/capture hooks and output suppression here; phase 05 connects the actual settings interface to these hooks.

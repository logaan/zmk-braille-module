---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Test chord timing and mode transitions
cargo:
  type: task
  status: draft
---

# Test chord timing and mode transitions

1. Exercise overlapping presses, early releases, held keys, late presses, repeated presses, and consecutive chords against the resolved lifecycle policies.
2. Verify all nine bits, space-only versus space-plus-dot, and exactly one resolution after both window expiry and participant releases.
3. Verify short/long gesture exclusivity, the dedicated key's exclusion from masks, and no unintended output across enable/disable or settings transitions.
4. Check conventional space, non-role keys, and conventional keymap operation when Braille input is off; test settings hooks without requiring the later UI.

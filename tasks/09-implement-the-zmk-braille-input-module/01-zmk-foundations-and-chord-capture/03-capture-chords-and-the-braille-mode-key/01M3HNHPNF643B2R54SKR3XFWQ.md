---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Capture chords and the braille-mode-key
cargo:
  type: task
  status: draft
---

# Capture chords and the braille-mode-key

1. Use ZMK behavior APIs; chords end on release, so no chord timer is needed.
2. The braille-mode-key and non-Braille keys never enter the chord mask.
3. Write each detector and key unit test alongside its implementation task; phases 05 and 06 verify hardware-in-the-loop behavior.

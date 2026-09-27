---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Detect and finalize nine-bit chords
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Detect and finalize nine-bit chords

1. Start on first press, accumulate only presses inside the window, wait for window expiry and all participant releases, emit one mask, then reset; support space-only and space-plus-dot.
2. Unit-test the detector alongside the implementation: all nine bits, space-only versus space-plus-dot, overlapping presses, early releases, held keys, late presses, repeated presses, consecutive chords, and exactly one resolution after window expiry and participant releases.

---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Detect and finalize nine-bit chords
cargo:
  type: task
  status: draft
---

# Detect and finalize nine-bit chords

1. Start on first press, accumulate only presses inside the window, wait for window expiry and all participant releases, emit one mask, then reset; support space-only and space-plus-dot.

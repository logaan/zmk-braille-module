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

1. Start a chord on the first `&bc`/`&bspc` press, add every Braille key pressed while any Braille key is still held, and emit one nine-bit mask when the last is released; then return to idle.
    1. Dots 1–8 are bits 1–8; Braille space is the ninth bit.
    2. Space-only, dot-only, and space-plus-dot masks are all ordinary chords.
2. The detector uses no timer.
3. Unit-test the detector alongside the implementation: all nine bits, space-only versus space-plus-dot, overlapping presses, early releases while others are held, presses joining a partly released chord, a key re-pressed within one chord, consecutive chords, a layer change mid-chord, and exactly one resolution on the last release.

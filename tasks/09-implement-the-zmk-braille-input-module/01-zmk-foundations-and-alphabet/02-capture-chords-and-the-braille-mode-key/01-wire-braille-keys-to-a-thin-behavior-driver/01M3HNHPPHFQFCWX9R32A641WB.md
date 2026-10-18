---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Wire Braille keys to a thin behavior driver
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Wire Braille keys to a thin behavior driver

1. Implement `src/behavior_braille.c` using ZMK behavior APIs: `&bc N` and `&bspc` press/release events feed the chord detector by binding parameter, at any position and on any layer.
2. Keep `&bspc` separate from a conventional `&kp SPACE`; non-Braille keys keep their conventional behavior and never join a chord.
3. The module is always on; there is no Braille on/off state to check.
4. Keep the driver thin, with no character or command dispatch.
5. Unit-test routing alongside the implementation: each `&bc` parameter and `&bspc` sets the right bit regardless of position or layer, conventional space and non-Braille keys pass through, and a release reaches the binding that received its press even after a layer change.

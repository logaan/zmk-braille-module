---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Wire physical roles to a thin behavior driver
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Wire physical roles to a thin behavior driver

1. Implement `src/behavior_braille.c` using ZMK behavior APIs and the physical-routing decision; accept eight dots and a distinct Braille space key at arbitrary positions.
2. Keep conventional space separate from the Braille space bit and leave non-role keys usable in the hybrid keymap.
3. Preserve conventional keymap operation when Braille input is disabled; do not mix chord collection with character/command dispatch.
4. Unit-test role routing alongside the implementation: arbitrary dot/space positions, Braille space versus conventional space, non-role keys, and passthrough when Braille input is off.

---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Add supported noncharacter factory bindings
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Add supported noncharacter factory bindings

1. Add per-mode rules for Tab, Return, Delete, Escape, arrows, Caps Lock, Shift, Control, Option, Command, and F1–F12, using the researched chords with their source comments.
2. Bind Shift, Control, Option, and Command as ZMK sticky-key actions (`&sk`).
3. Add Globe/Fn and media/system actions only where the host-capability research found them representable.
4. Unit-test the bindings alongside the data: each function in each initial mode, and sticky modifiers combining with the next Braille output.

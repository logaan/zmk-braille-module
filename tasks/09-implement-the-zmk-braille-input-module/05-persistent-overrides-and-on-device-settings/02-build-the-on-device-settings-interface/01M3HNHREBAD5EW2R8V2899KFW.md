---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Build the on-device settings interface
cargo:
  type: task
  status: draft
---

# Build the on-device settings interface

1. Use compact, text-oriented ZMK display widgets on the central half's display.
2. Suspend Braille host typing while settings captures input; share capture, Grade 1 interpretation, and the action representation instead of introducing another detector or translator.
3. Every screen is driven by the settings controls defined in the first child task: navigation chords plus mnemonic letter shortcuts.
4. Integrate storage and the existing settings-entry, mode, and pending-state APIs; the child tasks implement the flow once.
5. Write state-machine and search unit tests alongside each task; the hardware tasks that follow verify on-display behavior.

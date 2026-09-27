---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Implement entry and mode selection
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Implement entry and mode selection

1. Open settings from phase 01's settings-entry action and leave through the leave-settings control.
2. Present only compiled character modes and apply a selection immediately, without a confirmation prompt.
3. Persist the selected mode under the module's settings key and restore it on boot.
    1. Use the keyboard's default mode when no mode is saved or the saved mode is not compiled in.
4. Discard pending input on entry, and keep settings input away from host typing; character-mode selection is independent of layers.
5. Unit-test alongside the implementation: entry through the settings action and exit, immediate application without confirmation, listing compiled modes only, pending input discarded on entry, restoring the saved mode, and falling back to the default mode when none is saved or the saved mode is missing.

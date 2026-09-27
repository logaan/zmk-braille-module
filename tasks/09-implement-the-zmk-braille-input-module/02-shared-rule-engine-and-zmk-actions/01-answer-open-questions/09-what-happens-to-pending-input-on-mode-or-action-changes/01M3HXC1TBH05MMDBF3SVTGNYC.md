---
created: Mon 28 Sep 2026 - 03h 07m 29s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: 'Decided: discard pending input on any transition'
cargo:
  type: task
  status: delivered
---

# Decision: discard on anything

## 1. Decision

1. Any of these discards pending multi-chord input and resets UEB context (numeric, capitalization, grade 1 indicator state) without host output:
    1. A layer change.
    2. A non-Braille key press.
    3. A character-mode change.
    4. A command action, such as navigation or a noncharacter function.
    5. Settings entry (already decided in phase 01).
2. A physical chord in progress still completes on its last release, because ZMK routes releases to the binding that received the press; only multi-chord input pending after that is discarded.
3. Accepted trade-off: a complete match waiting on a possible longer rule can be lost, e.g. releasing the braille-mode-key mid-symbol.

## 2. Impact

1. The engine observes layer changes and non-Braille key presses (ZMK event listeners) to trigger the discard.
2. `03-track-boundaries-and-pending-output`, the computer8 isolation task, and the pending-cell display implement and test it.

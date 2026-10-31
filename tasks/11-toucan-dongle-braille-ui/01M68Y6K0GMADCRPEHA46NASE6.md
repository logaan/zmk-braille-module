---
created: Sat 31 Oct 2026 - 23h 15m 54s
author:
  type: agent
  harness: codex
  model: gpt-6
subject: Original Toucan dongle braille dashboard
cargo:
  type: task
  status: started
  worker:
    type: agent
    model: gpt-6
    name: dongle-ui
---

1. Build an original UI for the Toucan Prospector dongle, independent of existing screen designs.
2. Display both keyboard-half battery percentages, distinguishing unknown or disconnected readings.
3. Show selected Bluetooth host channel or USB, connection state, and held modifiers.
4. Show the selected braille mode: computer8, ueb1, or ueb2.
5. In braille mode, preview the currently held dots using the active mode and context without sending host input.
    1. Dot 1 previews a and dots 13456 preview y when those cells mean those letters.
    2. Show ambiguous prefixes and contractions honestly in UEB2.
6. Display pending braille cells until committed or discarded.
7. Add host verification and build the pinned Toucan dongle firmware; physical display acceptance remains a hardware check.
8. Preserve existing unrelated peripheral-display work and update the Toucan module revision after delivery.

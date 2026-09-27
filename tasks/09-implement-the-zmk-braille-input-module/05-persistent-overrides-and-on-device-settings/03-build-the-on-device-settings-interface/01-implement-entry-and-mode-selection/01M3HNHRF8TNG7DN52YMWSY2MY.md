---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Implement entry and mode selection
cargo:
  type: task
  status: draft
---

# Implement entry and mode selection

1. Connect phase 01's long-press entry/exit hooks to settings; do not implement a second gesture detector.
2. Present only compiled character modes and apply selection immediately without a confirmation prompt.
3. Apply the shared pending-input/transition policy and isolate settings capture from host typing; do not confuse character-mode selection with Braille input on/off.

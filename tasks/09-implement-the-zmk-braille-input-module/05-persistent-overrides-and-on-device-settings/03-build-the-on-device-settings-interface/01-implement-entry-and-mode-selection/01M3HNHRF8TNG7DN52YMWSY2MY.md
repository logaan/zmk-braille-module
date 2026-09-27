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

1. Connect phase 01's settings-entry action and exit hooks to settings; settings opens from a rule-bound chord, not a long press.
2. Present only compiled character modes and apply selection immediately without a confirmation prompt.
3. Apply the shared pending-input/transition policy and isolate settings capture from host typing; do not confuse character-mode selection with layer changes.
4. Unit-test entry and mode selection alongside the implementation: entry through the settings action and exit, immediate application without confirmation, listing compiled modes only, and pending-input handling on entry and exit.

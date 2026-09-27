---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Invoke ordered ZMK behavior actions
cargo:
  type: task
  status: draft
---

# Invoke ordered ZMK behavior actions

1. Invoke compiled behavior bindings with appropriate ZMK press/release events and up to two parameters, preserving order and the resolved modifier lifecycle.
2. Keep dispatch generic for letters, space, navigation, and custom actions; ordinary text emits host keystrokes, not Unicode Braille cells.
3. Provide the integration boundary for phase 05's local Grade 1 search input without executing captured/settings input on the host or introducing a second action representation.

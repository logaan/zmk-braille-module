---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Persistent overrides and on-device settings
cargo:
  type: task
  status: draft
---

# Persistent overrides and on-device settings

1. Let users create and change chord-to-action bindings on the keyboard, without a host tool or reflash.
2. Persist the user bindings and the selected character mode across reset and power cycle.
3. Use compact, text-oriented ZMK display widgets on the central half's display: the QBF's 128×32 OLED and the Toucan's nice!view.
4. Storage depends on phase 02's shared schema; the interface also uses phase 01's capture and settings-entry hooks, Grade 1 search input, and the compiled named modes.
5. Deliver pending-input display feedback for the initial modes here; optional Grade 2 reuses the same interface.

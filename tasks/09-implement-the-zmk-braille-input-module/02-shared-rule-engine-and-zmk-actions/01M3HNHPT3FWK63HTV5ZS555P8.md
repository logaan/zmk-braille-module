---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Shared rule engine and ZMK actions
cargo:
  type: task
  status: draft
---

# Shared rule engine and ZMK actions

1. Build one matcher and action path for character modes, navigation, and overrides (spec §4 and §6.2–6.3).
2. Depend on phase 01's chord-event contract and the relevant rule/context decisions, not on finished character tables or persistent storage.
3. Validate the engine with fixture tables first; settle factory assignments together with the character mappings and verify real-table combinations in phases 03–06.

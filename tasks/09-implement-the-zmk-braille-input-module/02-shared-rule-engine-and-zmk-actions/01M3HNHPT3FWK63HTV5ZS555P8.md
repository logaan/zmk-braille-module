---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Shared rule engine and ZMK actions
cargo:
  type: task
  status: draft
---

# Shared rule engine and ZMK actions

1. Build one matcher and action path for character tables, per-mode factory commands, and user overrides.
2. Depend on phase 01's chord events, not on finished character tables or persistent storage.
3. Validate the engine with fixture tables; verify real-table combinations in phases 03, 04, and 06.

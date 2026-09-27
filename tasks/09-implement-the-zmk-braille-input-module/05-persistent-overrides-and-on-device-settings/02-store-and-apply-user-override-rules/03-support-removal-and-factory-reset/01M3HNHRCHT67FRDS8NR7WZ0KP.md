---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Support removal and factory reset
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Support removal and factory reset

1. Delete one override to expose the factory mapping again, or clear all overrides; avoid altering factory tables.
2. Unit-test removal and reset alongside the implementation: deleting one override restores factory behavior, clearing all overrides leaves factory tables intact, and neither touches unrelated ZMK settings.

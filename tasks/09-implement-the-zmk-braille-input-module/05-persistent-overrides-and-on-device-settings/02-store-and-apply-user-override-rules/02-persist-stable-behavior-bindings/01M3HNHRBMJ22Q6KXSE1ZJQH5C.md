---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Persist stable behavior bindings
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Persist stable behavior bindings

1. Use a dedicated ZMK/Zephyr settings key; serialize stable behavior identifiers and up to two parameters per action, never device pointers.
2. Persist the full common rule shape (up to 8 chords, the position constraint, and up to 16 ordered actions) at variable length, so short rules stay small within the settings partition.
3. Resolve identifiers to live compiled devices on load.
4. Apply the resolved invalid-record policy, and report save failure to the UI rather than presenting an unsaved edit as durable.
5. Unit-test persistence alongside the implementation: round-tripping multi-chord rules with ordered multi-action bindings, both parameters, and constraints; identifier resolution rather than pointer reuse; invalid records and unavailable identifiers; and survival across simulated reset and power cycle.

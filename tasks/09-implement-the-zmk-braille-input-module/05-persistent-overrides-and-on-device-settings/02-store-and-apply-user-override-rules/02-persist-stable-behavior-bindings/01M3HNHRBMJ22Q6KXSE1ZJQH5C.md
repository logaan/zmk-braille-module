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

1. Use a dedicated ZMK/Zephyr settings key; serialize each action as its behavior's compiled device name string, such as `key_press`, and up to two parameters, never device pointers.
2. Persist the full common rule shape (up to 8 chords, the position constraint, and up to 16 ordered actions) at variable length, so short rules stay small within the settings partition.
3. Resolve device names to live compiled devices on load.
4. Load each saved record independently.
    1. Skip a record that is corrupt, fails validation, uses an unsupported storage format, or names a behavior that is not compiled in; valid records still load.
    2. Leave skipped records in storage, so a record whose behavior returns in a later firmware works again.
    3. Rebinding the same input, removing that override, or resetting all overrides replaces or deletes a skipped record.
    4. Show a display notice at boot when any record was skipped.
    5. Never alter factory rules.
5. Report save failure to the UI rather than presenting an unsaved edit as durable.
6. Unit-test persistence alongside the implementation: round-tripping multi-chord rules with ordered multi-action bindings, both parameters, and constraints; device-name resolution rather than pointer reuse; skipping corrupt, invalid, unsupported-format, and unresolvable records while valid ones load; skipped records staying stored and loading again once their behavior is compiled in; replacement and deletion of skipped records; the boot notice; and survival across simulated reset and power cycle.

---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Persist stable behavior bindings
cargo:
  type: task
  status: draft
---

# Persist stable behavior bindings

1. Use a dedicated ZMK/Zephyr settings key; serialize stable behavior identifiers and up to two parameters per action, never device pointers.
2. Persist the full common rule shape, including chord sequences, constraints, and ordered actions; resolve identifiers to live compiled devices on load.
3. Apply the agreed invalid-record policy and report save failure to the UI rather than presenting an unsaved edit as durable.

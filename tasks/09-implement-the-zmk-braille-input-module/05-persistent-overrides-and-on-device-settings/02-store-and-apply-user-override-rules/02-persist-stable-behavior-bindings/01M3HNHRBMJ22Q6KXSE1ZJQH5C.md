---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Persist stable behavior bindings
cargo:
  type: task
  status: draft
---

# Persist stable behavior bindings

1. Use a dedicated ZMK/Zephyr settings key; serialize stable identifiers and up to two parameters per action rather than device pointers, then resolve devices on load.

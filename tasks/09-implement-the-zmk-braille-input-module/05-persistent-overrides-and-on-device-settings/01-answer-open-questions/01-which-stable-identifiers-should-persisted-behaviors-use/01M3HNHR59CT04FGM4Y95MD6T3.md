---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Which stable identifiers should persisted behaviors use?
cargo:
  type: task
  status: draft
---

# Which stable identifiers should persisted behaviors use?

1. Choose stable identifiers that can be resolved to compiled behavior devices on load, never serialized device pointers.
2. Record behavior when an identifier is no longer available; survival across reset/power cycle is required, not an unconditional migration guarantee across arbitrary firmware changes.

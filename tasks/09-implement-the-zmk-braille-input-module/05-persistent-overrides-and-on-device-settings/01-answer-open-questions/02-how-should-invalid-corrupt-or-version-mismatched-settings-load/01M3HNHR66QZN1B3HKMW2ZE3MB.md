---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: How should invalid, corrupt or version-mismatched settings load?
cargo:
  type: task
  status: draft
---

# How should invalid, corrupt or version-mismatched settings load?

1. Define validation and recovery for invalid saved rules, unresolvable behavior identifiers, and unsupported stored formats without altering factory rules.
2. Keep failure handling bounded; this question does not require a general settings-migration framework.

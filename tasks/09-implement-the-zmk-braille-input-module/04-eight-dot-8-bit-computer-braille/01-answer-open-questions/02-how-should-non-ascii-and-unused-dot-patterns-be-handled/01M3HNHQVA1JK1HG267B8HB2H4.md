---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: How should non-ASCII and unused dot patterns be handled?
cargo:
  type: task
  status: draft
  worker: user
---

# How should non-ASCII and unused dot patterns be handled?

1. Decide explicit mapped/reserved entries and reserved-input behavior for patterns outside the printable ASCII mapping.
2. Spec §6.1 requires accounting for all 256 patterns, not implementing additional character sets or a host action for every entry.

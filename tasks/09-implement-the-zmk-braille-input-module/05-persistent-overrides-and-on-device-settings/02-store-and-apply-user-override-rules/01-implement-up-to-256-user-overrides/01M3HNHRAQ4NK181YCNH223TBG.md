---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Implement up to 256 user overrides
cargo:
  type: task
  status: draft
---

# Implement up to 256 user overrides

1. Support 256 added/replacement rules in a separate, empty-by-default layer using the common shape: chord sequences, position constraints, and ordered actions.
2. Keep overrides available in every character mode and prefer them to factory mappings with the same input under the shared matching policy.
3. Preserve read-only factory data and enforce capacity/sequence/action bounds without silently overwriting unrelated saved rules.

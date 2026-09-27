---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Test persistence and precedence
cargo:
  type: task
  status: draft
---

# Test persistence and precedence

1. Test empty defaults, 256-rule capacity and its boundary, added rules, replacement precedence, and shorter/longer sequence interactions.
2. Round-trip multi-chord rules with ordered multi-action bindings, both parameters, and constraints; verify behavior resolution rather than pointer reuse.
3. Test invalid records and unavailable identifiers, persistence across reset/power cycle, deletion, and full override reset.
4. Verify overrides remain available in both initial character modes, and removal restores factory behavior without mutating factory tables or unrelated ZMK settings.

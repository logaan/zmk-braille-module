---
created: Mon 28 Sep 2026 - 00h 50m 49s
author: user
subject: Display pending cells until committed
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Display pending cells until committed

1. Use the common engine's pending-state interface to show delayed initial-mode input on the normal compact display, including unresolved Grade 1 sequences.
2. Update or clear displayed cells in step with commitment, fallback, and discards on layer, key, mode, command, or settings transitions; never show them as already committed host output.
3. Show brief notices for dropped incomplete prefixes and reserved computer8 patterns.
4. Test bounded pending-state rendering and notices, and reserve the same interface for optional Grade 2; this is not a separate contraction display implementation.

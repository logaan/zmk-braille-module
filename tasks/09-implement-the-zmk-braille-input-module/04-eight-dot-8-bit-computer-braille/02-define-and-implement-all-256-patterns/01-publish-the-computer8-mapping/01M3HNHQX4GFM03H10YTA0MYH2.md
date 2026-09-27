---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Publish the computer8 mapping
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Publish the computer8 mapping

1. Document the selected eight-dot encoding, independent dot 7 and dot 8 bits, and all 256 mapped/reserved entries.
2. Explain the zero-dot entry and physical space-only input without changing the spec's default literal-space behavior or requiring an impossible empty chord.
3. Include printable ASCII coverage and reserved-entry behavior; reserve does not mean that every nonprintable pattern needs a new host action.

---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: When should an ambiguous rule prefix flush?
cargo:
  type: task
  status: draft
  worker: user
---

# When should an ambiguous rule prefix flush?

1. Define bounded pending-input/latency behavior for complete matches that are also prefixes, incomplete sequences, unmatched input, and buffer exhaustion.
2. Chords are release-terminated with no timer, so any inter-chord latency bound introduced here is the only input timeout.
3. Coordinate with the word-end and transition questions; preserve longest-valid matching and shorter fallback without losing subsequent cells.

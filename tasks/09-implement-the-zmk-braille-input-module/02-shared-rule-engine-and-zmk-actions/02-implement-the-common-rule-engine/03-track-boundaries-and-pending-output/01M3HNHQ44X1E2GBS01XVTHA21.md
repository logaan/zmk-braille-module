---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Track boundaries and pending output
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Track boundaries and pending output

1. Implement word-position constraints and the agreed UEB context, bounded lookahead, and commit/fallback policies in the common engine.
2. Apply the agreed mode/action/settings transition policy without leaking stale cells or context into the next input session.
3. Expose pending cells and their commit/cancel lifecycle for phase 05's display; do not defer all delayed-input feedback to Grade 2.
4. Unit-test context and pending output alongside the implementation: all word positions, ambiguous and unmatched prefixes, buffer and latency limits, and transition contracts that leave no stale cells or context.

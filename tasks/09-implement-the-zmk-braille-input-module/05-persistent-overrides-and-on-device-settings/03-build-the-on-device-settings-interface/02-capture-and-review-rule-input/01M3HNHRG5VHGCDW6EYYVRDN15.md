---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Capture and review rule input
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Capture and review rule input

1. Use the normal detector to capture a chord or bounded short sequence and show its detected dot/space combinations.
2. Apply the resolved UI control for completing capture; distinguish it from keys belonging to the recorded input.
3. For an existing factory/user mapping, offer override, re-record, and cancel before changing any saved rule.
4. Unit-test capture state alongside the implementation: single chords, bounded sequences, the completion control, and the override/re-record/cancel choices for an existing factory or user mapping.

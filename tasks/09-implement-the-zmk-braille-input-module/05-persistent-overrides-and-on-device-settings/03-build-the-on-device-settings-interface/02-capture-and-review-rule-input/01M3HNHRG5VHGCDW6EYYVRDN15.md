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

1. Use the normal chord detector to capture a chord or a sequence of up to 8 chords, and show the detected dot and space combinations.
2. Apply the resolved control for completing capture; distinguish it from keys belonging to the recorded input.
3. For input that already has a factory or user mapping, offer override, re-record, and cancel before changing any saved rule.
4. Unit-test capture state alongside the implementation: single chords, sequences up to the bound, the completion control, and the override, re-record, and cancel choices.

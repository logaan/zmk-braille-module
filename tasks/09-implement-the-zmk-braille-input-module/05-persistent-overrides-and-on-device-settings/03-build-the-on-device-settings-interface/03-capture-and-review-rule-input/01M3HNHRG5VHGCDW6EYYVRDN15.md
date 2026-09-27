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
2. After each captured chord, show the sequence so far and prompt to add another chord or finish.
    1. The prompt is a menu driven by the settings controls and shortcuts; its input is never recorded.
    2. Every chord entered while capture is waiting for a chord is recorded, including chords that match settings controls.
    3. Capture finishes without prompting after the eighth chord.
3. For input that already has a factory or user mapping, offer override, re-record, and cancel before changing any saved rule.
4. Unit-test capture state alongside the implementation: single chords, sequences up to the bound, the add-another/finish prompt, recording chords that match settings controls, automatic finish at eight chords, and the override, re-record, and cancel choices.

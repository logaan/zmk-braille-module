---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Match positional Grade 2 rules
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Match positional Grade 2 rules

1. Reuse the existing `word-start` constraint (the `word_start` field) and longest-valid matching; `word-middle` is `after-alphabetic` and `before-alphabetic` together.
2. Add the rule conditions of `references/research/ueb-grade2-word-boundaries.yaml` (`normative.conditions`):
    1. `standing-alone`: the sign is a letters-sequence of its own under rules 2.6.1–2.6.4, bounded by a space, hyphen, or dash with only the listed punctuation and indicators between.
    2. `after-alphabetic` and `before-alphabetic`: the neighbouring sign is a letter or contraction, not a capitals indicator or terminator.
    3. `no-lower-contact`: the immediate neighbours, disregarding capitals indicators, are not lower punctuation, a hyphen, a dash, or a quotation mark.
    4. `shortform-word`: a shortform inside a longer standing-alone word qualifies under rules 10.9.2–10.9.5, checked against the Shortforms List data.
3. Resolve each buffered word as a whole, following `back_translation.procedure` in the YAML:
    1. Read left to right with the `ueb1` context rules, trying the contraction rows whose conditions hold, longest first, else the `ueb1` meaning.
    2. Then read shortforms in each standing-alone letters-sequence that qualifies, preferring the reading with the most qualifying shortforms.
    3. Output the `ueb1` (uncontracted) meaning wherever no contraction's condition holds.
4. Treat the Braille space, Return, and Tab chords as spaces for standing alone; a buffer resolved at 32 cells ends at a word boundary, and its next cell begins a word.
5. Keep all resulting text and actions on the shared binding-dispatch path.
6. Unit-test positional matching alongside the implementation: each condition holding and failing, longest-valid match with shorter and uncontracted fallback, every case in `normative.ambiguous_sequences`, and shared dispatch of the resulting actions.

---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Extend common context and buffering
cargo:
  type: task
  status: draft
---

# Extend common context and buffering

1. Extend the phase 02 engine only with the word and word-part context needed for ICEB Grade 2, not a new capture, matching, or dispatch path.
    1. Reuse the context fields `numeric`, `caps`, and `word_start`, their class transition table, and the `word-start` constraint of `references/research/ueb-indicator-context.yaml`.
    2. Add the rule conditions `standing-alone`, `after-alphabetic`, `before-alphabetic`, `no-lower-contact`, and `shortform-word` defined in `references/research/ueb-grade2-word-boundaries.yaml`; each is decided within one buffered symbols-sequence.
2. Write each extension unit test alongside its implementation task; reuse phase 05's pending-cell display and the common transition policy.

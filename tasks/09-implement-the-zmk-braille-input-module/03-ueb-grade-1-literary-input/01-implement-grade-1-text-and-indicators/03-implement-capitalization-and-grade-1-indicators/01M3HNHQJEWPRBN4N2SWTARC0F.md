---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Implement capitalization and grade 1 indicators
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Implement capitalization and grade 1 indicators

1. Implement capitalisation as specified in `references/research/ueb-capitalisation.yaml` and grade 1 indicators as specified in `references/research/ueb-grade-1-indicators.yaml`, using the `caps` and `word_start` context fields of `references/research/ueb-indicator-context.yaml`.
2. Dot 6 followed by a letter is that letter in uppercase; it also ends a capitalised word.
3. Dots 6, 6 capitalise letters until a space, a single capital letter, a non-alphabetic sign, or the capitals terminator.
4. Dots 6, 6, 6 capitalise letters, across spaces, punctuation, and numbers, until the capitals terminator; dots 6, 6 inside it leave it in effect.
5. Dot 6 then dot 3 is the capitals terminator; with no capitals mode in effect it does nothing.
6. The grade 1 indicators (dots 5, 6 once, twice, or three times, and dots 5, 6 then dot 3) are accepted anywhere and produce no output; each ends numeric mode and a capitalised word, and the sign after it does not begin a word.
7. Indicators are context, never printable keys.
8. Unit-test alongside the implementation: single capitals, capitalised words and each thing that ends them, capitalised passages across spaces, punctuation, and numbers, the terminator mid-word and with no mode in effect, each grade 1 indicator, no indicator emitted as a printable key, and an indicator followed by a chord that cannot continue it.

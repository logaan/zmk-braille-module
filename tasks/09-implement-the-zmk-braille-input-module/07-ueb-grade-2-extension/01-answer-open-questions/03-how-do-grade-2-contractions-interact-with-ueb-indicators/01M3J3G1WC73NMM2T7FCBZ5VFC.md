---
created: Mon 28 Sep 2026 - 04h 54m 32s
author:
  type: agent
  harness: claude-code
  model: claude
subject: No contractions in grade 1 mode; capitals cover contractions
cargo:
  type: task
  status: done
---

# Result

1. Summary
    1. No contraction is read in grade 1 mode (2.5.1); every sign then takes its `ueb1` meaning.
    2. Grade 1 mode comes from ⠰ (the next symbol), ⠰⠰ (to a space or ⠰⠄), ⠰⠰⠰ (to ⠰⠄), or a numeric indicator (to a space, hyphen, dash, or ⠰⠄); ⠰⠄ ends each.
    3. A grade 1 indicator marks letters rather than contractions for a standing-alone letter that is an alphabetic wordsign (not a, i, o), letters that would read as a shortform (10.9.7–10.9.9), a-j after a digit, and letters after a hyphen that follows a number.
    4. It also marks punctuation that would read as a contraction (7.1.3), a question mark that would read as his or a quote, and standing-alone ⠠⠦ ⠠⠴ ⠠⠶.
    5. ⠰ after a letter or contraction before e, g, l, n, s, t, or y is a final-letter groupsign.
    6. Numeric-indicator grade 1 mode outlasts numeric mode, so contractions never follow a number in the same word part (1st, houses4lease).
    7. Capitals: the capital prefix capitalizes a contraction's first letter; capitalised words and passages cover contractions; adjacent capitals indicators block lower and final-letter groupsigns but not standing alone.
    8. `ueb2` needs two context fields, `grade1` and `grade1_symbol`; the existing numeric, capitals, and grade 1 indicator effects are unchanged.
2. Detail: `references/research/ueb-grade2-indicator-interactions.yaml`.
3. Tasks updated
    1. `07-.../02-extend-common-context-and-buffering`: the grade 1 fields and the `grade1-off` condition.
    2. `07-.../02-.../01-match-positional-grade-2-rules`: grade 1 mode tracking and tests.
    3. `07-.../03-.../01-add-wordsigns-and-strong-contractions` and `02-add-groupsigns-and-short-form-words`: `grade1-off`, rule classes, capitalization, grade 1 indicator tests.
    4. `07-.../03-.../03-test-iceb-contracted-and-fallback-examples`: indicator fixtures and the `ueb1` and `ueb2` contrast.

---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Implement other ICEB literary signs
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Implement other ICEB literary signs

1. Add the signs in the `additional_signs` section of `references/research/ueb-literary-signs.yaml`:
    1. `"`: dots 2, 3, 6 at the start of a word; dots 3, 5, 6; dots 4, 5 then 2, 3, 6; dots 4, 5 then 3, 5, 6; and dot 6 then 2, 3, 5, 6.
    2. `'`: dot 6 then 2, 3, 6; dot 6 then 3, 5, 6; and dot 3.
    3. `|`: dots 4, 5, 6 then 1, 2, 5, 6.
    4. `` ` ``: dots 4, 6 then 1, 6.
2. Dots 2, 3, 6 is an opening quotation mark only where the context's word start is set, and a question mark elsewhere.
3. Add no rule for the grave accent modifier (dots 4, 5 then 1, 6) or for the signs under `not_keyboard_characters`.
4. Dots 7 and 8 stay free in `ueb1` for nonliterary and custom chords.
5. Unit-test alongside the implementation: each added sign, dots 2, 3, 6 at and away from a word start, the grave accent modifier producing no output, and dots 7 and 8 unused by literary rules.

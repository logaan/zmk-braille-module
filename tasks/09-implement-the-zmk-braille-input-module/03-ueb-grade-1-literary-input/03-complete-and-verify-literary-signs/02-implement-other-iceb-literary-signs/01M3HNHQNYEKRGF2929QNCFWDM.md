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

1. Add the additional literary signs from the milestone inventory, including quotation marks, following the rulebook.
2. The grave accent is a UEB diacritic (dots 4, 5 then 1, 6); it does not make a standalone literary backtick key.
3. There is no literary vertical bar; backtick and vertical bar are reached through `computer8` or custom actions, and no UEB sign is invented for either.
4. Dots 7 and 8 stay free in `ueb1` for nonliterary and custom chords.
5. Unit-test alongside the implementation: the added signs including quotation marks, the grave diacritic, and dots 7 and 8 unused by literary rules.

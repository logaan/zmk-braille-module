---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: UEB Grade 1 literary input
cargo:
  type: task
  status: draft
---

# UEB Grade 1 literary input

1. Implement from the rulebook research in `references/research/`: `ueb-literary-signs.yaml`, `ueb-numeric-mode.yaml`, `ueb-capitalisation.yaml`, and `ueb-grade-1-indicators.yaml`, on phase 02's context representation in `ueb-indicator-context.yaml`.
2. Extend phase 01's lowercase alphabet into uncontracted UEB from the pinned ICEB rulebook, using phase 01's engine and actions and phase 02's context and command support.
    1. Uncontracted UEB is not a one-cell-to-one-character translation: indicators and multi-cell signs carry context.
3. Limit literary-sign work to keyboard input for the completeness target; do not build a general Braille-document or technical-notation translator.
4. Incomplete prefixes, such as the capital indicator or the first cell of a multi-cell sign, follow the shared pending-input policy with no Grade 1-specific dispatch: when the next chord cannot continue one, drop it with a display notice and interpret that chord from scratch.

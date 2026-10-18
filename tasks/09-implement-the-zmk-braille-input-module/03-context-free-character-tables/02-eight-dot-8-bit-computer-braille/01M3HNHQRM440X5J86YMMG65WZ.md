---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Eight-dot (8-bit) computer Braille
cargo:
  type: task
  status: draft
---

# Eight-dot (8-bit) computer Braille

1. Provide `computer8`, a separate named eight-dot character table covering printable US ASCII with one cell per character.
    1. It uses the US eight-dot computer Braille table that macOS VoiceOver uses.
    2. It does not apply UEB indicators, word-position rules, or contraction buffering.
    3. Dots 7 and 8 are independent bits, not UEB capital or numeric indicators.
2. Every one of the 256 dot patterns has an explicit mapping or reserved entry.
    1. A reserved pattern sends nothing to the host and briefly shows as unassigned on the display; user overrides may bind it.
3. `computer8` has its own factory command table from phase 02's research; collisions between command chords and character cells, such as dot 7 or 8 alone, are resolved explicitly and documented.

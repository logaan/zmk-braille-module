---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Which eight-dot encoding will define the documented ASCII map?
cargo:
  type: task
  status: draft
---

# Which eight-dot encoding will define the documented ASCII map?

1. Choose and document a consistent eight-dot map covering printable US ASCII with one cell per character, independently of UEB indicators.
2. Coordinate with phase 02's shared factory chord allocation before either table is finalized.
3. Account for all 256 dot patterns, including zero dots; physical Braille-space-only must emit literal space by default (spec §6.2), and an empty press sequence is not a chord.

---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Which eight-dot encoding will define the documented ASCII map?
cargo:
  type: task
  status: draft
  worker: user
---

# Which eight-dot encoding will define the documented ASCII map?

1. Choose and document a consistent eight-dot map covering printable US ASCII with one cell per character, independently of UEB indicators.
2. Map the 256 dot-only patterns; Braille-space-only emits a literal space and space-plus-dot chords remain the factory navigation and function pool, so the pools do not collide.
3. Account for the zero-dot entry: an empty press sequence is not a chord, so it can only be a reserved table entry.

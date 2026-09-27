---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Which collision-free space-plus-dot chords invoke navigation?
cargo:
  type: task
  status: draft
  worker: user
---

# Which collision-free space-plus-dot chords invoke navigation?

1. Assign factory chords for left/right, line beginning/end, and previous/next UI element using the selected host's VoiceOver-style behaviors.
2. Draw assignments from the space-plus-dot pool: dot-only patterns belong to the active character table, and Braille-space-only already emits a literal space (spec §6.2).
3. Keep navigation usable in every character mode and free of conflicts with the noncharacter function chords.

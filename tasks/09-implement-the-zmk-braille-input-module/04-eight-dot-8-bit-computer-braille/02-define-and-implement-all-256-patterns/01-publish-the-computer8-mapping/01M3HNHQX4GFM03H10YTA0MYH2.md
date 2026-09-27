---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Publish the computer8 mapping
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Publish the computer8 mapping

1. Identify the US eight-dot computer Braille table macOS VoiceOver uses and cite the source; document it, the independent dot 7 and dot 8 bits, and all 256 mapped/reserved entries.
2. Explain the zero-dot entry and physical space-only input without changing the default literal-space behavior or requiring an impossible empty chord.
3. Include printable ASCII coverage and reserved-entry behavior: a reserved pattern sends nothing to the host and briefly shows as unassigned on the display, and user overrides may bind it.
4. Document how command chords from phase 02 research that collide with cells (e.g. dots 7 or 8 alone) are resolved.

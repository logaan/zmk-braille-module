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

1. Identify the US eight-dot computer Braille table macOS VoiceOver uses and cite the source; document it, the independent dot 7 and dot 8 bits, and all 256 mapped or reserved entries.
2. Explain the zero-dot entry: an empty press is not a chord, so it is a reserved entry, while Braille-space-only emits a literal space.
3. Include printable ASCII coverage and reserved-entry behavior: no host output, a brief display notice, and availability to user overrides.
4. Document how command chords that collide with cells, such as dot 7 or 8 alone, are resolved.

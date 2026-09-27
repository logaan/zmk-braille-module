---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Which chords invoke navigation and settings?
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Which chords invoke navigation and settings?

1. Research and record factory chords, for each initial character mode, for:
    1. Left and right movement.
    2. Beginning and end of line.
    3. Previous and next UI element.
    4. The settings-entry action.
2. Follow the user's source precedence:
    1. macOS VoiceOver's Braille keyboard commands for that mode: six-dot for `ueb1`, eight-dot for `computer8`.
    2. Android TalkBack where VoiceOver has no equivalent.
    3. Hable One where neither has one.
3. Use dot-only chords where the source does, not only space-plus-dot; record each conflict with that mode's character table and its resolution.
4. Give every chord a source note (product, mode, and document or URL); the factory data carries it as a comment.
5. Keep navigation available in every character mode and free of conflicts with the noncharacter chords.

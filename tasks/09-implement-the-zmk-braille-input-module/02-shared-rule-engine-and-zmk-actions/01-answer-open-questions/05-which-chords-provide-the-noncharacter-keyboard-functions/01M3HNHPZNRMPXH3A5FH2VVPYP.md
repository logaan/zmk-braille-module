---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Which chords provide the noncharacter keyboard functions?
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Which chords provide the noncharacter keyboard functions?

1. Research and record factory chords, for each initial character mode, for Tab, Return, Delete, Escape, arrows, Caps Lock, Shift, Control, Option, Command, F1–F12, and the Globe/Fn and media/system functions that question 10 finds representable.
2. Follow the same source precedence as question 04: macOS VoiceOver for that mode, then Android TalkBack, then Hable One.
    1. Dots 7 and 8 for Delete and Return are expected, in line with VoiceOver; confirm whether VoiceOver uses them alone or with space.
3. Give every chord a source note (product, mode, and document or URL); the factory data carries it as a comment.
4. These are ZMK behavior bindings, not literary UEB signs.
5. Check the combined allocation for conflicts across navigation, both character tables, and user overrides.

---
created: Mon 28 Sep 2026 - 03h 14m 29s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: Research per-mode command chords
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Research per-mode command chords

1. Research factory chords, for each initial mode (`ueb1` and `computer8`), for:
    1. Left and right movement, beginning and end of line, and previous and next UI element.
    2. The settings-entry action.
    3. Tab, Return, Delete, Escape, arrows, Caps Lock, and F1–F12.
    4. Shift, Control, Option, and Command, as sticky modifiers.
    5. The media/system functions and Globe listed as supported in `references/research/zmk-globe-fn-and-media-actions.yaml`: display brightness down and up, Mission Control, Spotlight, previous track, play/pause, next track, mute, volume down and up, and Globe as a sticky modifier.
2. Take each chord from the first source that defines it:
    1. macOS VoiceOver's Braille keyboard commands for that mode: six-dot conventions for `ueb1`, eight-dot for `computer8`.
    2. Android TalkBack.
    3. Hable One.
3. Use VoiceOver's dots 7 and 8 for Delete and Return, alone or with space as VoiceOver defines them.
4. Use the chords VoiceOver uses by default for its sticky modifiers.
5. Chords may be dot-only where the source uses them; record every conflict with that mode's character table and its resolution.
6. Record each chord's source (product, mode, and document or URL) for the factory data's comments.
7. Deliver the table, with sources, in the delivery message; phase 06 publishes it.

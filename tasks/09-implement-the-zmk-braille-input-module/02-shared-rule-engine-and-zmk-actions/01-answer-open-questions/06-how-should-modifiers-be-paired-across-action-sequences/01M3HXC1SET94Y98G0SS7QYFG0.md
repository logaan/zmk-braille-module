---
created: Mon 28 Sep 2026 - 03h 07m 29s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: 'Decided: ZMK sticky modifiers on VoiceOver chords'
cargo:
  type: task
  status: delivered
---

# Decision: ZMK sticky modifiers on VoiceOver's chords

## 1. Decision

1. Modifier functions are ZMK sticky-key actions (`&sk LSHFT`, `&sk LCTRL`, `&sk LALT`, `&sk LGUI`) in ordinary rules.
2. Their factory chords are the ones macOS VoiceOver uses by default for its sticky modifiers; question 04/05 research records the source.
3. Behavior need not match VoiceOver exactly: ZMK's sticky-key semantics apply, e.g. stacking and release after the next key.

## 2. Impact

1. No custom modifier lifecycle in the engine; modifiers are ordinary behavior actions.
2. The dispatcher must be verified to release keycodes that ZMK sticky keys see as the "next key", so a sticky modifier applies to the following Braille output.

---
created: Sun 18 Oct 2026 - 19h 09m 10s
author:
  type: agent
  harness: pi
  model: gpt-5.6-terra
  name: task-reorg
subject: Direct factory chords
cargo:
  type: task
  status: draft
---

# Direct factory chords

## 1. Purpose

1.  Configure the factory keyboard-action chords that the delivered capture,
    matcher, table registry, and ordered action dispatcher can already execute.
2.  Treat this as rule-data, behavior-binding, and verification work; it does
    not require a new chord-capture, matching, dispatch, or context mechanism.

## 2. Delivered prerequisites

1.  A completed chord reaches the selected mode's command table and can invoke
    ordered `&kp` and `&sk` actions.
2.  Each mode already owns a separate command-table view and whitespace-chord
    metadata.
3.  The direct-action fixtures prove space, Return, Tab, and navigation can use
    the same path as alphabet rules.

## 3. Scope

1.  Replace the fixtures with researched factory rules for navigation, editing,
    ordinary keyboard functions, modifiers, and supported media/system actions.
2.  Use the existing `tests/braille/commands/voiceover.yaml` catalog as the
    candidate fixture set; confirm its sources before making a rule factory
    data.
3.  Make the resulting rules available in `ueb1` now and retain per-mode tables
    for the `computer8` table in phase 03.

## 4. Required first acceptance

1.  Braille Tab is dots 2, 3, 4, and 5 plus Braille space.
2.  Its nine-bit mask is `0x11e` and its factory action is `&kp TAB`.
3.  Tests must prove that this exact chord emits Tab and no alphabet character.

## 5. Deferred work

1.  Do not assign a factory settings-entry chord until the settings UI and its
    exit path exist in phase 05.
2.  Context-sensitive UEB signs, numeric/capital state, and word boundaries
    remain in phase 04.
3.  Static UEB symbols and the `computer8` character table are phase 03.

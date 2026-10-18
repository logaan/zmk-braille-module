---
created: Mon 28 Sep 2026 - 00h 50m 45s
author: user
subject: Implement the ZMK Braille input module
cargo:
  type: task
  status: draft
---

# Implement the ZMK Braille input module

## 1. Scope

1. Build chorded Braille input as an out-of-tree ZMK module; keyboards include it through `zmk-config` without changing ZMK core.
2. Target hybrid keyboards: conventional keymaps with Braille keys on whichever layers the keymap author chooses.
3. Phases 01–06 deliver the initial feature set:
    1. `ueb1`: uncontracted Unified English Braille (UEB) Grade 1.
    2. `computer8`: eight-dot computer Braille.
    3. Navigation and other ZMK actions.
    4. Persistent on-device chord bindings and selected character mode.
    5. Display feedback for settings and pending input.
4. Phase 07 adds `ueb2` (UEB Grade 2 contractions) as an optional extension; the initial feature set must not depend on it.
5. Other named Braille systems, such as Japanese or Spanish Braille, must be addable as named character tables without changing chord detection, matching, or navigation; none is required.
6. The completeness target is the printable characters and ordinary key functions of a US MacBook Air keyboard.
    1. Letters in both cases, digits, punctuation, and space.
    2. Tab, Return, Delete, Escape, arrows, Caps Lock, Shift, Control, Option, Command, and F1–F12.
    3. Globe/Fn where the host and ZMK can represent it.
    4. Host media and system functions, such as brightness and volume, where ZMK and host behaviors exist; they are actions, not Braille characters.
    5. Literary UEB signs cover the characters UEB defines; `computer8` and action bindings cover the rest.
    6. `references/01-mba-keys.yaml` is a supporting inventory only; it omits F1–F12 and Globe/Fn, which remain required.
7. Intended users are ZMK hobbyists, Braille learners, and sighted users of Perkins-style keyboards; feedback is visual, on an OLED or comparable display.
8. Out of scope: non-English tables, Touch ID, power, sound or haptics, a universal behavior editor, and a host configuration tool.

## 2. Authority

1. UEB follows *The Rules of Unified English Braille*, third edition 2024 (ICEB): `references/iceb/rules-of-unified-english-braille-2024.pdf`.
    1. Its provenance and license are in `references/README.md`.
    2. The book covers both grades; select uncontracted requirements using its sections 2.5 and 5.11.
    3. Secondary guides, such as BANA's *ABCs of UEB* and the UEB contractions summary, do not supersede it.
2. Factory command chords follow macOS VoiceOver's Braille keyboard conventions for each mode, then Android TalkBack, then Hable One; each factory rule carries a comment naming its source.

## 3. Baseline

1. Build, test, and accept on two keyboards.
    1. QBF Paintbrushes (`../qbf-paintbrushes/basic-zmk`): two Paintbrush PCBs on nice!nano v2 as a wireless split, 16 keys, a 128×32 SSD1306 OLED on each half; primarily a Braille device, with Braille keys on the base layer.
    2. beekeeb Toucan (`../zmk-keyboard-toucan`): seeeduino_xiao_ble split with a nice!view gem display on the left (central) half; primarily QWERTY, with a braille-mode-key giving home-row Braille for numbers and symbols.
2. The module and both keyboards use one pinned ZMK revision: the most recent ZMK release.
3. Both keyboards are splits; the module runs on the central half, and feedback appears on the central half's display.
4. The host is macOS with VoiceOver and a US input layout.

## 4. Core behavior

1. Keymaps use ten kinds of Braille key.
    1. `&bc 1` to `&bc 8`: Braille dots 1–8.
    2. `&bspc`: Braille space, distinct from any conventional space key.
    3. braille-mode-key(target-layer): hold keeps the target layer active while chording; tap switches to it.
2. The module is always on: Braille input happens whenever Braille keys are reachable on the active layers.
3. A chord is the nine-bit mask of every Braille key pressed from the first press until the last release; there is no timer.
4. A rule matches up to 8 chords and invokes up to 16 ordered ZMK behavior actions, each a behavior device with up to two parameters.
    1. Letters, space, navigation, and custom actions share this one representation.
    2. Text is sent as ordinary host keystrokes, not Unicode Braille.
    3. Rules may carry a word-position constraint: `any`, `word-start`, `word-middle`, or `word-end`.
5. The engine matches the active character table, that mode's factory command table, and the user override layer.
    1. It prefers the longest valid sequence; a user override wins over a factory rule with the same input.
    2. Ambiguous input stays pending until the next chord extends it or rules it out.
    3. An incomplete prefix that the next chord rules out is dropped with a display notice, and that chord is interpreted from scratch.
    4. A whitespace chord (the Braille space, or a Return or Tab command chord) commits pending input as at a word end, then acts.
    5. Any layer change, non-Braille key press, character-mode change, other command action, or settings entry discards pending input and resets UEB context.
6. Up to 256 user overrides and the selected character mode persist across reset and power cycle; overrides are edited on the keyboard.

## 5. Execution model

1. One agent executes the plan sequentially, phase by phase and task by task; no parallel branches are assumed.
2. Phases with unresolved questions begin with an `01-answer-open-questions` decision milestone.
    1. Resolve the whole milestone before starting that phase's implementation tasks.
    2. When a question is answered, update the affected tasks so they read as final requirements, then mark the question done and archive it.
3. `worker` is set on leaf tasks only; phase and group folders are containers.
    1. `worker: user`: preference, policy, limit, and hardware-in-the-loop acceptance tasks.
    2. `worker: agent`: implementation, unit tests, documentation, audits, and research from the ICEB rulebook, ZMK/Zephyr, or screen-reader documentation.
    3. The user reviews every task and marks it ready; agents do not start draft tasks or mark their own work ready.
4. Unit tests are written alongside the implementation they cover, inside the same task.
    1. A standalone test task is used only for additional effort that justifies itself: cross-component integration, hardware-in-the-loop, rulebook-example conformance suites, or exploratory testing.
5. Work may be paused and resumed from the repository and task messages alone.
    1. Do not add handoff notes, progress journals, or status reporting beyond the TaskDown delivery message each task already requires.

## 6. Order and dependencies

1. Phase 01 delivers the first end-to-end lowercase alphabet input.
    1. Pin the baseline and build the module, Braille key bindings, release-terminated chord capture, the braille-mode-key, and the settings-entry action.
    2. Build the common rule and action data, matcher, ordered action dispatch, and named-table registration; then implement the 26 six-dot alphabet cells immediately after those prerequisites.
    3. Use fixture command tables, character modes, and in-memory overrides where their real data arrives later; lowercase alphabet input does not depend on context transitions or populated factory command tables.
    4. Finish with the user-owned alphabet hardware test on the baseline keyboards before phase 02.
2. Phase 02 extends that same engine with context transitions, word boundaries, and the pending-input lifecycle, then researches and implements per-mode factory command chords.
    1. Its UEB-context representation is `references/research/ueb-indicator-context.yaml`, derived from phase 03's rulebook research in `references/research/`.
    2. Braille-space output, navigation, and the other factory commands arrive here, after alphabet bring-up.
3. Phase 03 extends the existing alphabet into complete `ueb1` literary input; phase 04 builds `computer8`. Both use phase 01's engine and actions and phase 02's context and command support, and check their real tables against the command tables.
4. Phase 05 storage depends on phase 01's common schema; its settings interface also needs phase 01's capture and settings-entry hooks, Grade 1 search input, and the compiled named modes.
5. Phase 06 verifies the combined feature set on the baseline keyboards and completes user documentation; it is not the first build or test step.
6. Phase 07 reuses the delivered engine, display, capture, and actions, adding only contraction data and necessary word context.

## 7. Readiness and completion

1. Parent tasks summarize their children; no work is implemented twice.
2. Use fixture tables or in-memory overrides where a later component does not exist yet; verify real combinations when both sides are built.
3. Maintain one key-coverage checklist through phases 03, 04, and 06; record host-dependent exceptions rather than inventing literary signs.
4. Initial acceptance requires a core-unmodified ZMK build, host-keystroke and key-function coverage, usable display and settings flows, pending-input feedback, and survival of overrides and the selected mode across reset and power cycle.

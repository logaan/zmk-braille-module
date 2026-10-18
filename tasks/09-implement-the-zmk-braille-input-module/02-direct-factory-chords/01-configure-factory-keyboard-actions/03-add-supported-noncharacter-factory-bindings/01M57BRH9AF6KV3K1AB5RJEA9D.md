---
created: Sun 18 Oct 2026 - 22h 18m 43s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
subject: 'delivered: sourced keys sticky modifiers and media'
cargo:
  type: task
  status: delivered
---

# Noncharacter factory bindings

## 1. Revised scope

1. The user approved skipping missing assignments after checking `references/`; the preceding answer records that decision.
2. Host Caps Lock, F1-F12, brightness down/up, Mission Control, Spotlight, previous/next track and system mute remain unassigned, not invented or falsely labelled supported factory chords.
3. The source audit, README and cheatsheet list these omissions. Settings entry remains deferred to Phase 05.

## 2. Implementation

1. Expanded `src/braille_rules_nav.c` to 25 sourced rules per mode, including Escape, all Mac Delete/Return aliases, sticky Shift/Control/Option/Command/Globe, Consumer volume and TalkBack play/pause. Every row names its source.
2. Globe uses Mac\u2019s Fn position with the project-required Consumer GLOBE substitution; it does not implement true Fn. No held/toggle modifier or new chord timer is added.
3. Module devicetree retains `&kp` and `&sk` via real phandle references. Compile-time BASIC-consumer warning preserves the default FULL configuration.
4. Added two optional Return-alias masks to existing whitespace metadata; dot8 and space+dot8 now share the canonical Return\u2019s pending-text path rather than discarding it as navigation.
5. Added `scripts/test-commands` and its native fixture: production bindings, capture, matcher, tables, dispatcher, unmodified ZMK sticky/key-press behaviors and HID. A test-only report observer always forwards to the real endpoint.
6. Updated `references/braille-cheatsheet.typ`, rebuilt its PDF, and added command-reference checks. Preserved the user\u2019s no-layout-review commit instruction in `AGENTS.md`; generated PDFs are marked binary in `.gitattributes`.

## 3. Verification

1. All 32 module/reference host tests passed with the pinned Kconfig parser and PyYAML environment; log: `build/baseline/logs/phase02-module-host.log`.
2. ASan/UBSan command and engine tests passed, including split off/on and optional Grade 2 command-data isolation.
3. `./scripts/test-commands` passed both modes, every direct key, every sticky modifier with a following letter and exact Tab, one-shot release, keyboard-modifier stacking, line-start/end and previous/next-UI compositions, Globe+e, and unused Shift/Globe timeouts.
4. `./scripts/smoke-build` passed all seven targets: both halves of both keyboards, Toucan computer8, and both reset images; checks include retained kp/sk devices and FULL consumer usages. No hardware was flashed.
5. The unfiltered suite also encountered 8 unrelated existing coordinator-test errors: tmux calls target absent pane `%1`, and a pane-info mock has an obsolete response shape. Neither the coordinator nor its tests were changed.
6. Native POSIX endpoint errors are expected because it has no physical USB host. BLE/macOS behavior and hardware acceptance are not claimed.

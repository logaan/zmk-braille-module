---
created: Sun 18 Oct 2026 - 21h 54m 28s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
subject: 'delivered: verified command subset and source gaps'
cargo:
  type: task
  status: delivered
---

# Command-source audit

## 1. Result

1. `references/research/factory-command-chords.yaml` records the full verified table, exact masks, source sections, and per-mode collision decisions.
2. Read the full primary Mac VoiceOver article (`https://support.apple.com/en-us/103560`), TalkBack keystrokes (`https://support.google.com/accessibility/android/answer/3535886?hl=en`), both Hable manuals, and the iOS article used by the candidate catalog.
3. Validated all 22 selected masks and their uniqueness with PyYAML; no search-summary-only assignments were promoted.

## 2. Table for both initial modes

1. Braille space alone: `0x100` -> `&kp SPACE` (project requirement).
2. Mac VoiceOver Navigation: space+1 `0x101` -> Left; space+4 `0x108` -> Right; space+3 `0x104` -> Up; space+6 `0x120` -> Down.
3. Mac VoiceOver Navigation: space+123 `0x107` -> Home; space+456 `0x138` -> End; space+12 `0x103` -> Escape.
4. Mac VoiceOver Scrolling: space+3456 `0x13c` -> Page Up; space+1456 `0x139` -> Page Down.
5. Mac VoiceOver Braille: space+2345 **`0x11e` -> exactly `&kp TAB`**; space+15 `0x111` -> Return; space+145 `0x119` -> Backspace (Mac Delete).
6. Mac VoiceOver 8-dot: dot7 `0x040` and space+7 `0x140` -> Backspace; dot8 `0x080` and space+8 `0x180` -> Return.
7. Mac VoiceOver 8-dot: space+47 `0x148` -> sticky Shift; space+37 `0x144` -> sticky Control; space+27 `0x142` -> sticky Option; space+17 `0x141` -> sticky Command.
8. Mac VoiceOver Fn position: space+57 `0x150` -> `&sk GLOBE`, the required supported-action substitution, explicitly not true Fn.
9. Compose sticky Command then Left/Right for macOS line start/end, and sticky Shift then Tab for previous UI element. Home/End are not macOS line-start/end shortcuts.

## 3. Boundaries

1. Space-bearing rules cannot collide with dot-only character rows; dot7/8 are outside UEB six-dot input and non-printable in the planned computer8 mapping, and must remain command-only there. Real Phase-03 rows still require collision validation.
2. Settings entry, held/toggle modifiers, iOS-only candidate editing shortcuts, and true Fn semantics are not installed.

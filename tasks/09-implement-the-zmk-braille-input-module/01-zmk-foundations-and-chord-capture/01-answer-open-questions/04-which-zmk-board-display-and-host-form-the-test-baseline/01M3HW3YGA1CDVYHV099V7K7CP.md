---
created: Mon 28 Sep 2026 - 02h 45m 35s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: 'Decided: QBF and Toucan on one pinned ZMK release'
cargo:
  type: task
  status: delivered
---

# Decision: QBF Paintbrushes and Toucan on one pinned ZMK release

## 1. Keyboards

1. QBF Paintbrushes (`../qbf-paintbrushes/basic-zmk`)
    1. Two Paintbrush PCBs on nice!nano v2 as a wireless split: 16 keys.
    2. A 128×32 SSD1306 OLED on each half.
    3. Primarily a Braille device: dots on the base layer's `asdf jkl;` row.
2. beekeeb Toucan (`../zmk-keyboard-toucan`)
    1. seeeduino_xiao_ble split; nice!view gem display on the left (central) half.
    2. Primarily QWERTY: a braille-mode-key gives home-row Braille for numbers and symbols instead of bespoke sym/num/nav layers.
3. Both are splits: the module runs on the central half, and display feedback targets the central's display.

## 2. ZMK revision

1. The module targets one pinned ZMK revision: the most recent ZMK release when the pinning task runs.
2. QBF moves from unpinned `main` to that release.
3. Toucan moves from `v0.3` to that release if its community modules (cirque input module, rgbled widget, nice_view_gem) allow; record any blocker.

## 3. Host

1. macOS with VoiceOver; the QBF keymap already binds VoiceOver and the virtual Braille keyboard.
2. US input layout, matching the MacBook Air coverage target.

## 4. Impact

1. New task `02-build-the-module-and-behavior-binding/01-pin-zmk-and-the-keyboard-configs`.
2. The smoke build, phase 05 hardware tests, and phase 06 acceptance use both keyboards.

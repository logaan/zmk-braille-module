---
created: Sun 18 Oct 2026 - 22h 18m 43s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
subject: 'blocked: user hardware acceptance; firmware built'
cargo:
  type: task
  status: blocked
  worker: user
---

# Hardware acceptance handoff

## 1. Status

1. Phase-02 software is delivered with the user-approved skipped defaults recorded in the source audit and cheatsheet.
2. All seven smoke targets compiled and linked; native ZMK verifies the real key/sticky/HID path. No physical keyboard was flashed or host behavior observed by the agent.
3. This user-owned task remains blocked on physical macOS/BLE acceptance, not on source-assignment decisions.

## 2. Local firmware

1. QBF central: `build/baseline/build/smoke-qbf-left/zephyr/zmk.uf2`.
    1. SHA-256: `b57a78a86a1c73c6322e0be300fd79175a41191401c88368b111d7972e279fae`.
2. Toucan central: `build/baseline/build/smoke-toucan-left/zephyr/zmk.uf2`.
    1. SHA-256: `c7a215f2fd3134d5d776e00d5c416080ee3c2498264d066292eb9b4a01f476d2`.
3. Matching peripheral builds are `smoke-qbf-right` and `smoke-toucan-right`; no module code runs on those halves.
4. Rebuild with `./scripts/smoke-build`; these artifacts use this local checkout through manifest-based discovery. Published sibling module pins have not been changed or pushed. Do not use the settings-reset images for this acceptance unless intentionally clearing keyboard settings.

## 3. Checks on both keyboards

1. Use macOS, US input layout and record VoiceOver/Quick Nav state. Exercise Toucan\u2019s momentary and toggled Braille-layer entry.
2. Tab: Braille Space + dots 2345 emits one Tab; the same dots without Space emit lowercase `t` in ueb1. Old fixture Space+45 must not emit Tab.
3. Confirm literal space, Return (Space+15, dot8, Space+8), Delete (Space+145, dot7, Space+7), Escape, all arrows and page navigation. Commands must not type alphabet characters.
4. Sticky Shift+Tab and Control+Tab apply once; the next Tab is plain. Command then Left/Right is macOS line start/end. Home/End are not those shortcuts.
5. Confirm stacked Shift+Command with Left; default unused modifiers expire after 1000 ms. Slow chording may need an &sk timeout adjustment, not a chord-capture timer.
6. Sample volume down/up (Space+126 / Space+345) and play/pause (Space+78). Record actual host behavior.
7. Globe (Space+57) then `e` should send Globe+e; record host effect and unused timeout. It is not Fn; do not expect Fn+Delete or Fn+arrow behavior.
8. No factory defaults exist in this phase for Caps Lock, F1-F12, brightness, Mission Control, Spotlight, previous/next track or system mute, per the user\u2019s skip decision. Settings entry, literary context and persistence are outside this test.
9. Record keyboard/host setup, each result and any host-specific limitation here before accepting the phase.

---
created: Mon 28 Sep 2026 - 02h 45m 35s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: 'Decided: ten key kinds bound in the keymap'
cargo:
  type: task
  status: delivered
---

# Decision: ten key kinds bound in the keymap

## 1. Decision

1. The module defines ten kinds of key, bound in the keymap like any ZMK behavior.
    1. `&bc N`, where N is 1–8: Braille dot N (`#binding-cells = <1>`).
    2. `&bspc`: Braille space, the ninth chord bit; distinct from a conventional `&kp SPACE`.
    3. braille-mode-key(target-layer): see task `03-capture-chords-and-the-braille-mode-key/03-implement-the-braille-mode-key-and-settings-action`.
2. A key's role is wherever the keymap author places these bindings, on any positions and layers.
    1. QBF Paintbrushes: dots on the base layer, since it is primarily a Braille device.
    2. Toucan: dots on a layer reached with the braille-mode-key, since it is primarily QWERTY.
3. The module is always on; there is no module-level Braille on/off state.
    1. Braille input happens whenever `&bc`/`&bspc` bindings are reachable on the active layers.
4. Non-Braille keys keep their conventional bindings and never join a chord.

## 2. Supersedes

1. The seed spec's parameterless binding (`#binding-cells = <0>`) listing nine role positions on one node.
2. The seed spec's dedicated Braille key with short/long press gestures.

## 3. Impact

1. `02-build-the-module-and-behavior-binding/03-define-the-braille-key-devicetree-bindings` defines `&bc`, `&bspc`, and the mode key.
2. `03-capture-chords-and-the-braille-mode-key/01-wire-braille-keys-to-a-thin-behavior-driver` routes by binding parameter, not position.

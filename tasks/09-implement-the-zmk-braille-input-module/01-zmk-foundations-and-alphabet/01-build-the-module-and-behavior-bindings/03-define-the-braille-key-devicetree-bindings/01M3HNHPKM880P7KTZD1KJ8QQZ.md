---
author: user
cargo:
  status: draft
  type: task
  worker:
    harness: pi
    model: unknown
    type: agent
created: Mon 28 Sep 2026 - 00h 50m 46s
subject: Define the Braille key devicetree bindings
---

# Define the Braille key devicetree bindings

1.  Add bindings under `dts/bindings/behaviors/` and default behavior nodes in a
    module `.dtsi`, so keymaps can use:
    1.  `&bc N`, where N is 1–8: Braille dot N (`#binding-cells = <1>`); reject
        other values where the build allows.
    2.  `&bspc`: Braille space (`#binding-cells = <0>`).
    3.  The braille-mode-key, taking its target layer as the parameter.
    4.  The settings-entry action (`#binding-cells = <0>`), invocable from
        rules.
2.  Provide a per-keyboard default character mode by name, defaulting to `ueb1`;
    it applies when no selected mode is saved or the saved mode is not compiled
    in.
3.  Chords have no timeout property: they end only when every Braille key is
    lifted.
4.  Record the chosen compatibles and the mode-key label in the delivery
    message.

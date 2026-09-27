---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Define the parameterless Braille devicetree binding
cargo:
  type: task
  status: draft
---

# Define the parameterless Braille devicetree binding

1. Add `dts/bindings/behaviors/` for `zmk,behavior-braille`: `#binding-cells = <0>`, dot-one through dot-eight, space-key, 50 ms default timeout, and default mode `ueb1`.

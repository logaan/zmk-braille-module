---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Define the parameterless Braille devicetree binding
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Define the parameterless Braille devicetree binding

1. Add the `zmk,behavior-braille` binding under `dts/bindings/behaviors/` with `#binding-cells = <0>`.
2. Configure nine physical roles, `dot-one` through `dot-eight` and `space-key`, plus per-keyboard `timeout-ms` (default 50) and named `mode` (default `ueb1`).

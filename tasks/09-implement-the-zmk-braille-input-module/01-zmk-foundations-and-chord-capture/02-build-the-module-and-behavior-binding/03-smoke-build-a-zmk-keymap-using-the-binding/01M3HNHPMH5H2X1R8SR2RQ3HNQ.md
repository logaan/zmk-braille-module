---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Smoke-build a ZMK keymap using the binding
cargo:
  type: task
  status: draft
---

# Smoke-build a ZMK keymap using the binding

1. Use the selected baseline to check module discovery, the enable option, and parameterless binding in a real west build.
2. Check physical-role properties, the 50 ms/default-ueb1 configuration, and configurable timeout/mode values at the build/configuration boundary.
3. Do not require completed character tables or settings screens for this smoke build; verify runtime routing in the capture tests and actual modes in phases 03–06.

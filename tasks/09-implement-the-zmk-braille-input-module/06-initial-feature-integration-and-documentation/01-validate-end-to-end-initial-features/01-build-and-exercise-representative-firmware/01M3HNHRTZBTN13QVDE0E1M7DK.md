---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Build and exercise representative firmware
cargo:
  type: task
  status: draft
  worker: user
---

# Build and exercise representative firmware

1. Verify out-of-tree discovery and installation through `zmk-config`, devicetree configuration, behavior/keymap integration, and display widgets without ZMK-core changes.
2. Exercise the module disabled at build time, Braille keys unreachable on the active layers, active Braille typing (base layer and braille-mode-key hold and tap), and settings capture as distinct states.
3. Check conventional keys/space and press/release ownership across transitions, both initial modes, and delayed-input feedback on the selected display.

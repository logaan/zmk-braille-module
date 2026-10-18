---
created: Sun 18 Oct 2026 - 12h 41m 29s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: leaf-01-baseline
subject: Both keyboards pass after owned-code release port
cargo:
  type: task
  status: started
  worker:
    type: agent
    harness: pi
    model: gpt-6.1-sol
    name: leaf-01-baseline
---

1. All six baseline builds now pass: Paintbrush left/right/settings_reset and Toucan left/right/settings_reset; no Braille module mounted or enabled.
2. Only Paintbrush-owned drawing/preview code was ported to LVGL 8 and zmk_endpoints_selected; community dependencies and ZMK core remain clean. Both displays and all normal firmware features remain enabled.
3. Host preview: all six frames are byte-identical to the saved original LVGL 9 preview output; default and reverse rotations pass all pixel checks. Reverse rotation host build passes AddressSanitizer/UndefinedBehaviorSanitizer.
4. Final validation and repository commits are in progress. The module currently has no CI workflow; west/west.yml remains the authoritative pinned development/CI manifest, with firmware build wiring left to later module tasks.

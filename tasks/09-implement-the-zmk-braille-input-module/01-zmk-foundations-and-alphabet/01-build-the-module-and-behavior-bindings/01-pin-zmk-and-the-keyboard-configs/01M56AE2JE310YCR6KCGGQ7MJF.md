---
created: Sun 18 Oct 2026 - 12h 36m 17s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: leaf-01-baseline
subject: Release builds expose keyboard-owned LVGL mismatch
cargo:
  type: task
  status: started
  worker:
    type: agent
    harness: pi
    model: gpt-6.1-sol
    name: leaf-01-baseline
---

1. Latest release verified independently via GitHub releases/latest and git ls-remote: v0.3.0, edf5c0814fd3ea202e43aad2d68fd32e882a518c. Kagi lacks credentials.
2. Fresh Docker 3.5 release workspace: Toucan left (Studio, rgbled_adapter, nice_view_gem) and right both build successfully with upstream community modules unchanged; Paintbrush settings_reset also passes.
3. Both Paintbrush halves fail in their keyboard-owned OLED code: LVGL 9 lv_layer_t and LV_COLOR_FORMAT_L8 APIs are absent from release LVGL 8.3.7. Release board/toolchain configuration has been corrected to nice_nano_v2 / zmk-build-arm:3.5.
4. Following the coordinator clarification, minimally port the Paintbrush-owned drawing and its existing host preview to the release APIs, preserving both required displays. No ZMK core or community dependency changes.

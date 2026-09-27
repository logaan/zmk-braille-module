---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Update installation and keymap documentation
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Update installation and keymap documentation

1. Replace hello-world claims and stale links in root `README.md`; document module installation through `zmk-config` without core changes and the enable option.
2. Provide working keymap examples for both baselines: `&bc 1`–`&bc 8` and `&bspc` on any positions and layers, the braille-mode-key, and a distinct conventional space key.
3. Explain the dot layout, release-terminated chords, the `ueb1` default, compiled modes, restoring the last selected mode on boot, braille-mode-key hold and tap, the settings chord, and conventional/Braille operation.
4. Explain that settings search lists only behaviors compiled into the firmware, and that a keymap keeps an otherwise unreferenced built-in behavior with `#define ZMK_BEHAVIORS_KEEP_<NAME>` or `ZMK_BEHAVIORS_KEEP_ALL` before including `behaviors.dtsi`.
5. Document display/pending feedback, the settings control and shortcut table, the full binding/capture/search/parameter/confirmation flow, override precedence/capacity, persistent saves, skipped-record notices, removal, and reset.

---
created: Mon 28 Sep 2026 - 03h 54m 20s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: Iterate zmk_behavior_ref; greedy subsequence top-K search
cargo:
  type: task
  status: done
---

# Result

1. Summary
    1. Researched ZMK `v0.3.0` (`edf5c08`) on Zephyr `v3.5.0+zmk-fixes`.
    2. Every behavior, whether built-in, keymap-defined, or from a module, registers a `zmk_behavior_ref` in a ROM iterable section; `STRUCT_SECTION_FOREACH(zmk_behavior_ref, item)` enumerates them at runtime.
    3. `device->name` (for example `key_press`) is the stable identifier; `ZMK_BEHAVIOR_METADATA` adds display names (for example "Key Press") and parameter metadata.
    4. Unreferenced built-in behaviors are omitted from the build (`/omit-if-no-ref/`) unless the keymap defines `ZMK_BEHAVIORS_KEEP_*`.
    5. Search: case-insensitive, boundary-insensitive greedy subsequence scoring with a fixed top-K array; about 650 keycode names cost well under 1 ms per keystroke on the nRF52840.
2. Detail: `references/research/zmk-behavior-enumeration.yaml`.
3. Tasks updated
    1. `05-.../03-.../04-fuzzy-search-compiled-behavior-names`: enumeration, exclusions, matcher, ranking, display.
    2. `05-.../03-.../05-select-parameters-and-action-bindings`: parameter classification from metadata.
    3. `05-.../02-.../02-persist-stable-behavior-bindings`: name resolution and validation APIs.
    4. `06-.../02-.../01-update-installation-and-keymap-documentation`: compiled-behavior and keep-flag documentation.

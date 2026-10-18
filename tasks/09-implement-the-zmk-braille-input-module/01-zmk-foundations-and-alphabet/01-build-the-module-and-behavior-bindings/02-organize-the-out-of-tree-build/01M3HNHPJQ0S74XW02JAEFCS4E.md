---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Organize the out-of-tree build
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Organize the out-of-tree build

1. Update `zephyr/module.yml`, root `CMakeLists.txt`, and `Kconfig` to replace the hello-world stub (`src/braille.c`) with the behavior module and its `ZMK_FEATURE_BRAILLE` enable option.
2. Put implementation in `src/` and shared declarations in `include/`, keeping these responsibilities separate:
    1. `behavior_braille.c`: thin ZMK behavior driver.
    2. `braille_engine.c`: shared matcher and buffer.
    3. `braille_rules.h`: common rule definition.
    4. `braille_rules_grade1.c`, `braille_rules_nav.c`, and `braille_rules_computer8.c`: rule data.
    5. `braille_rules_grade2.c`: optional Grade 2 rule data.
3. Register the minimal behavior devices needed to link the smoke-test keymaps; event routing and capture follow in the next group.
4. Build through ordinary `zmk-config` module inclusion on the pinned release without patching ZMK core; the module runs on the split central.

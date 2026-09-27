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

1. Update `zephyr/module.yml`, root `CMakeLists.txt`, and `Kconfig` to replace the startup stub with the behavior module and its enable option.
2. Use `src/` and `include/` as needed, retaining the spec §4.3 separation of thin driver, shared engine, common rule definition, and rule-data files.
3. Register the minimal behavior device needed to link the smoke-test keymap; event routing and capture follow in the next group.
4. Build through an ordinary `zmk-config`/module inclusion without patching ZMK core; use the selected central or non-split baseline.

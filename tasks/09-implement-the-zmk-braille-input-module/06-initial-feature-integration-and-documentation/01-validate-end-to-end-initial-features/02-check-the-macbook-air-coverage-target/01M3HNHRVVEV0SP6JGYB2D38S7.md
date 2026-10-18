---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Check the MacBook Air coverage target
cargo:
  type: task
  status: draft
  worker: user
---

# Check the MacBook Air coverage target

1. Complete the shared checklist against the root task's completeness target, using `references/01-mba-keys.yaml` only as a supporting inventory.
2. Verify lowercase and uppercase letters, digits, all printable US ASCII including space, and the required literary signs through their intended input routes.
3. Verify factory bindings for Tab, Return, Delete, Escape, all arrows, Shift, Control, Option, and Command.
4. Check the Globe binding against `references/research/zmk-globe-and-fn-actions.yaml` on the host.
    1. It works over Bluetooth on both baseline keyboards.
    2. Globe followed by a Braille letter triggers its Globe shortcut, such as Globe+E for the emoji picker; record what a lone Globe action does.
    3. Sticky modifiers, including Globe, stay active long enough between chords.
    4. Record any failure as a host or ZMK limitation.
5. Confirm host keystrokes and usable modifier combinations on the US input layout, not merely matching masks in unit tests.

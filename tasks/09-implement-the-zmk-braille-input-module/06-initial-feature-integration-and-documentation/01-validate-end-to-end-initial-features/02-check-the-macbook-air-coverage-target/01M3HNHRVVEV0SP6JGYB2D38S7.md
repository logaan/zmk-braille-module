---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Check the MacBook Air coverage target
cargo:
  type: task
  status: draft
---

# Check the MacBook Air coverage target

1. Complete the single checklist against spec §1.4 and §6, using `references/01-mba-keys.yaml` only as a supporting inventory.
2. Verify lowercase/uppercase letters, digits, all printable US ASCII including space, and the required literary signs through their intended input routes.
3. Verify factory bindings for Tab, Return, Delete, Escape, all arrows, Caps Lock, Shift, Control, Option, Command, and F1–F12; YAML omissions do not remove requirements.
4. Check representable Globe/Fn and supported media/system behavior bindings; document genuine host/ZMK limitations and exclude Touch ID/power.
5. Confirm host keystrokes and usable modifier combinations on the chosen US input layout, not merely matching numeric masks in unit tests.

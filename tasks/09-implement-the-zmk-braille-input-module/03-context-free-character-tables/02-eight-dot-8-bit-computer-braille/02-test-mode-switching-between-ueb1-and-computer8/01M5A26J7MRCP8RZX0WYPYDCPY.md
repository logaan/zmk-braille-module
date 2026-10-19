---
created: Mon 19 Oct 2026 - 23h 29m 20s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
subject: Real-mode integration and shared coverage delivered
cargo:
  type: task
  status: delivered
---

1. `tests/host/test_computer8_modes.py` exercises both real character/command pairs through production selection, capture, matching and dispatch. Pending real UEB prefixes and override sequences cannot cross either switch direction; seeded context resets, failed selection preserves state, and shared overrides survive.
2. Verified mapped/reserved and multi-chord overrides, longest-match fallback, immediate ordinary Computer8 characters, every real factory command, literal space, and Tab/Return whitespace handling. Split/non-split, optional Grade-2 and pinned real keycode variants pass.
3. Added the shared `references/key-coverage.md`: Computer8 covers all 95 printable US ASCII characters; current UEB1 covers 55, with contextual/fallback work explicitly deferred. It also tracks keyboard functions, host-dependent Globe limits, visual feedback, settings/persistence and physical acceptance.
4. All nine Computer8 tests and all 41 host tests outside the independently reproduced legacy cheatsheet/coordinator failures pass. Native ZMK real HID/command/sticky checks pass. Both central-half UEB1 and Computer8 smoke builds pass for QBF and Toucan.
5. Added a test-only QBF Computer8 build target; both actual keyboard layouts retain their contract with the Computer8 boot default. Central UF2s are under `build/baseline/build/smoke-<target>/zephyr/zmk.uf2`. Production defaults/pins are unchanged; no board was flashed.
6. Updated the implementation cheatsheet and rebuilt its PDF. Settings-screen interaction, persistence, phase-04 UEB transitions and physical acceptance are not claimed.

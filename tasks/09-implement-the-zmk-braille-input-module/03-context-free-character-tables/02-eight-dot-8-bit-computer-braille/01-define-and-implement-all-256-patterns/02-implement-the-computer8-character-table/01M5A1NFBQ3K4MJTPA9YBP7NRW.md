---
created: Mon 19 Oct 2026 - 23h 20m 00s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
subject: All Computer8 ASCII cells implemented and verified
cargo:
  type: task
  status: delivered
---

1. Implemented all 94 printable non-space ASCII cells in `src/braille_rules_computer8.c`, with any predicates, neutral class, ordinary ZMK keycodes and implicit Shift. Existing capture, matching and dispatch are unchanged.
2. Exhaustive production-path tests pass for all 256 patterns, literal Braille space, raw HID usages, 159 enterable reserved notices/recovery, reserved-cell overrides, the full ASCII stream, and the published/YAML fixtures. Split/non-split, optional Grade-2 and real pinned ZMK keycode-header variants pass.
3. Native `scripts/test-commands` now requires real character data rather than Computer8 stand-in overrides; representative Computer8 HID, commands, sticky modifiers/Globe and timeout checks pass. QBF-left, Toucan-left and Toucan-Computer8 firmware smoke builds pass.
4. Updated the implementation reference and rebuilt its PDF. No device was flashed; visual unassigned presentation remains phase 05 and hardware acceptance remains user-owned.
5. The full host attempt found stale cheatsheet and coordinator failures; the same failures reproduce from starting commit `53e9439` in an isolated archive. They are unrelated to this implementation and are recorded in `references/README.md`.

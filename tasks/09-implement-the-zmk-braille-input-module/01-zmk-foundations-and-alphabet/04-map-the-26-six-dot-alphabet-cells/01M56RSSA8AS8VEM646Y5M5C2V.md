---
created: Sun 18 Oct 2026 - 16h 47m 21s
author:
  type: agent
  harness: pi
  model: openrouter/pareto-code
  name: alphabet-leaf-12
subject: Six-dot alphabet delivered; hardware handoff prepared
cargo:
  type: task
  status: delivered
---

# Alphabet mapping delivered

## 1. Implementation

1.  Completed only this leaf in commit `4ce0868` (`Map the 26 six-dot
    alphabet cells`).
2.  `src/braille_rules_grade1.c` now populates `braille_rules_ueb1_characters`
    with the 26 six-dot alphabet cells of the ICEB rulebook (section 2.5),
    in the one common rule representation: each letter is a single dot-only
    chord (dot n = bit n-1) invoking one `&kp` action with the letter's HID
    usage (a..z = 0x04..0x1D), no modifiers, class `BRAILLE_CLASS_LETTER`,
    `when` ANY — an ordinary lowercase host keystroke, never Unicode Braille.
3.  `src/braille_tables.c` adds the boot-time selection the shared path
    needed: a `SYS_INIT` (POST_KERNEL) selects the devicetree default mode
    (`ueb1` unless the zmk,braille node overrides it) so the alphabet is live
    in firmware without any settings action; selection failure is logged, not
    asserted. Host tests are unaffected (guarded by `__ZEPHYR__`).
4.  Stale comments updated to match the populated table
    (`include/braille_tables.h`, `tests/host/test_tables.c`); README/Kconfig
    status text now says lowercase alphabet input works.

## 2. Unit tests (alongside the implementation)

1.  New `tests/host/test_alphabet.c` + `test_alphabet.py` compile the test
    with the real engine, dispatch, registry, and rule-data units against a
    recording `zmk_behavior_invoke_binding` stub, with `-Wall -Wextra
    -Werror`, split and non-split, and run it:
    1.  Table shape: 26 rules, each one dot-only chord and one `&kp` binding
        with the expected usage, ANY context, letter class.
    2.  All 26 mappings: each chord resolves through selection/matching/
        dispatch to exactly one press/release pair of the letter usage (HID
        0x04..0x1D in a..z order, i.e. ordinary lowercase), event forwarded
        unchanged, nothing left pending.
    3.  `abcdefghijklmnopqrstuvwxyz` typed consecutively: 26 keystrokes in
        order (52 invocations).
    4.  Factory validation: the alphabet does not collide with the mode's
        fixture command table; `ueb1` is the devicetree default selection.
    5.  Six-dot cells outside the alphabet (dot 3, dot 6, dots 2,3, dots 2,5,
        dots 3,6, dots 1–6) emit nothing and leave nothing pending; the
        alphabet works afterwards.
2.  Exit 0 for `python3 tests/host/test_alphabet.py`, also under
    `CC='cc -fsanitize=address,undefined -fno-omit-frame-pointer'` with no
    ASan/UBSan diagnostics.
3.  Regressions: `test_tables.py`, `test_engine.py`, `test_dispatch.py`,
    `test_rules.py`, `test_chord.py`, `test_build.py`, `test_routing.py`,
    `test_settings.py`, `test_kconfig.py`, `test_devicetree.py`,
    `test_keyboards.py` all pass. `python3 -m unittest discover` shows the
    same 11 failures + 2 errors as the clean tree (pre-existing
    devicetree/kconfig/keyboards environment issues), with the new suite
    passing.
4.  Changed C files pass release-style `clang-format --dry-run --Werror`;
    `git diff --check` clean.

## 3. Hardware handoff (prepared in the 05 task, not flashed)

1.  Fresh firmware for both halves of both keyboards built with
    `./scripts/smoke-build qbf-left qbf-right toucan-left toucan-right` at
    commit `4ce0868`: all four pristine builds exit 0 and
    `tests/firmware/check_smoke.py` passes on all four; both central maps
    link the alphabet table and boot selection; peripherals compile no
    Braille sources.
2.  UF2s staged in `build/handoff/alphabet/` with SHA-256 sums; originals in
    `build/baseline/build/smoke-<target>/zephyr/zmk.uf2`.
3.  The full handoff — current published pins, artifact paths, flashing
    instructions, key layout, and expected-behavior boundaries — is recorded
    as a message in
    `tasks/09-implement-the-zmk-braille-input-module/01-zmk-foundations-and-alphabet/05-test-alphabet-input-on-real-hardware/01M56RRTMFNX3JP3NKRB9EK1WX.md`.
    Its status is unchanged (draft, user-owned); no hardware was flashed and
    phase 02 was not started.

## 4. Preserved unrelated work

1.  The uncommitted `AGENTS.md` edit, the untracked message in
    `02-capture-chords-and-the-braille-mode-key/01-wire-braille-keys-to-a-thin-behavior-driver/`,
    and the untracked `.babashka-pod-*.port` tooling file were left alone.
2.  Rechecked this task immediately before delivery; no newer coordinator
    messages were present. No blockers.

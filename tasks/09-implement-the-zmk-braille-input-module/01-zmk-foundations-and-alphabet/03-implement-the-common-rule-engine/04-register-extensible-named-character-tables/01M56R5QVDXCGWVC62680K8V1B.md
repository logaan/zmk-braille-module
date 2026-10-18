---
created: Sun 18 Oct 2026 - 16h 36m 24s
author:
  type: agent
  harness: pi
  model: openrouter/pareto-code
  name: alphabet-leaf-11
subject: Named character-table registration delivered
cargo:
  type: task
  status: delivered
---

# Named character-table registration delivered

## 1. Implementation

1. Completed only this leaf in commit `552a89c` (`Register extensible named character tables`).
2. New `include/braille_tables.h` + `src/braille_tables.c`: one registry of compiled named modes. Each `struct braille_mode` pairs a character table with its own factory command table and its whitespace chords (Braille space, Return, Tab).
    1. Exactly one character table is selected by name through `braille_tables_select(name)`: `ueb1` and `computer8` always, `ueb2` only when `CONFIG_ZMK_BRAILLE_GRADE2` compiles its rules in; unknown names return `-ENOENT`, factory collisions return `-EINVAL` and keep the previous selection (via `braille_engine_set_tables`).
    2. Selection is the single mechanism for both entry points: `braille_tables_select_default()` reads the zmk,braille node's `default-mode` devicetree property (falling back to `"ueb1"` when the node is absent; `-DBRAILLE_TABLES_DEFAULT_MODE` overrides in host tests) and calls `braille_tables_select`, which phase-05 settings changes will call the same way. Selection discards pending input and resets the UEB context, matching the plan's character-mode-change reset.
    3. The registry owns the (fixture-capacity, initially empty) in-memory user override layer and re-registers it with the engine on every selection, so overrides stay active in every mode; the matcher re-reads `count` each chord, so appended rules take effect immediately.
    4. Future named language tables are new registry entries plus a rule-data unit in the same common representation; capture, matching, and navigation are untouched.
3. `src/braille_rules_nav.c` now carries per-mode fixture factory command tables (each mode owns its own array): Braille space, Return, Tab, and previous/next navigation as ordinary `&kp` actions on space-bit chords that cannot collide with dot-only character cells; every chord is marked fixture pending phase-02's VoiceOver-convention research. Device names use the established `DEVICE_DT_NAME(DT_NODELABEL(kp))` pattern, compiling under both real ZMK and the host stubs.
4. `src/braille_rules_grade1.c` / `braille_rules_computer8.c` / `braille_rules_grade2.c` export their (still empty) character table views; the alphabet leaf populates `braille_rules_ueb1_characters` next, phases 03/04/07 the rest.
5. `CMakeLists.txt` builds `src/braille_tables.c`; `tests/host/test_build.py`'s source-selection contract updated to match.

## 2. Host verification

1. `python3 tests/host/test_tables.py` compiles `tests/host/test_tables.c` plus the registry, engine, and rule-data units with `-Wall -Wextra -Werror` (split and non-split) in four variants and runs them: registration by exact name (case-sensitive, NULL-safe), `ueb2` exposed only when compiled (`-DCONFIG_ZMK_BRAILLE_GRADE2`), each mode selecting its own command table with navigation/noncharacter rules committing through the shared engine, per-mode whitespace chords, the shared override layer winning in every mode (including over a factory rule with the same input), devicetree-default selection (`computer8` override; uncompiled default reported as `-ENOENT`), and mode switches discarding pending input without output. Exit 0.
2. `python3 tests/host/test_engine.py`, `test_dispatch.py`, `test_rules.py`, `test_chord.py`, and `test_build.py` all still pass.
3. `python3 -m unittest discover` in `tests/host`: same 11 failures + 2 errors as the clean tree (pre-existing devicetree/kconfig/keyboards environment issues), with the new registry test passing.

## 3. Preserved unrelated work

1. The uncommitted `AGENTS.md` edit and the untracked message in `02-capture-chords-and-the-braille-mode-key/01-wire-braille-keys-to-a-thin-behavior-driver/` were left alone; only this leaf's ten files were committed.

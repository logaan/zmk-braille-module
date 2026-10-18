---
created: Sun 18 Oct 2026 - 15h 50m 26s
author:
  type: agent
  harness: pi
  model: openrouter/pareto-code
  name: alphabet-leaf-10
subject: Ordered behavior dispatch delivered
cargo:
  type: task
  status: delivered
---

# Ordered ZMK behavior dispatch delivered

## 1. Implementation

1. Completed only this leaf in commit `49180a6` (`Invoke ordered ZMK behavior actions`).
2. New `src/braille_dispatch.c` + `include/braille_dispatch.h`: `braille_dispatch_rule` invokes a committed rule's `actions[0..action_count)` in rule order, each pressed then released through `zmk_behavior_invoke_binding`, forwarding each binding's behavior device, both parameters, and the chord-resolution event unchanged; on the first failing invocation the remaining actions are skipped and the error propagates.
3. Modifiers are ordinary ZMK `&sk` sticky-key actions: a sticky key's own release leaves it armed until the next host key release, so a modifier action applies to the next rule's output with no engine-specific modifier lifecycle.
4. Dispatch is fully generic: nothing branches on characters, commands, or chord masks, so letters, space, navigation, and custom actions share the one `struct zmk_behavior_binding` representation, ordinary text becomes host keystrokes (never Unicode Braille cells), and space-only, dot-only, and space-plus-dot masks are handled identically (dispatch never reads the rule's chords).
5. The strong `braille_engine_commit_rule` override in the same unit is the engine's single host-output path; `cause` (word-end/discard evidence) is ignored by dispatch and left for phase-02 context transitions.
6. Phase-05 boundary: the settings/search flow captures and reviews rules through the same `struct braille_rule` representation and simply never calls dispatch, so captured/settings input never executes on the host; no second action representation was introduced.
7. `CMakeLists.txt` now builds `src/braille_dispatch.c`; `tests/host/test_build.py`'s source-selection contract was updated to match.

## 2. Host verification

1. `python3 tests/host/test_dispatch.py` compiles `tests/host/test_dispatch.c` plus `src/braille_dispatch.c` with `-Wall -Wextra -Werror` (split and non-split) against a recording `zmk_behavior_invoke_binding` stub and runs it: action ordering (press/release pairs in rule order), zero/one/two parameters forwarded unchanged, a sticky `&sk` modifier committed as its own rule preceding and applying to the next rule's `&kp` output, identical handling of space-only/dot-only/space-plus-dot masks, zero-action indicator rules invoking nothing, and a failing action stopping dispatch with the error propagated. Exit 0.
2. `python3 tests/host/test_engine.py`, `test_rules.py`, and `test_chord.py` still pass.
3. `python3 -m unittest discover` in `tests/host` shows the same 11 failures + 2 errors as before this leaf (pre-existing devicetree/kconfig/keyboards environment issues).

## 3. Preserved unrelated work

1. The uncommitted `AGENTS.md` edit and the untracked message in `02-capture-chords-and-the-braille-mode-key/01-wire-braille-keys-to-a-thin-behavior-driver/` were left alone; only this leaf's six files were committed.

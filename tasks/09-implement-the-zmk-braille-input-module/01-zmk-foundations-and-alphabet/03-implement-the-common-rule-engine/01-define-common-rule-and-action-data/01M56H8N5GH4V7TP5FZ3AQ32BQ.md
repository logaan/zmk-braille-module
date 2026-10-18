---
created: Sun 18 Oct 2026 - 14h 35m 40s
author:
  type: agent
  harness: pi
  model: openrouter/pareto-code
  name: alphabet-leaf-08
subject: Common rule and action data delivered
cargo:
  type: task
  status: delivered
---

# Common rule and action data delivered

## 1. Implementation

1. Completed only this leaf in commit `5bc213a` (`Define the common Braille rule and action data schema`).
2. Replaced the placeholder `include/braille_rules.h` with the single common schema.
    1. `struct braille_rule`: up to `BRAILLE_RULE_MAX_CHORDS` (8) nine-bit chord masks and up to `BRAILLE_RULE_MAX_ACTIONS` (16) ordered `struct zmk_behavior_binding` actions, each a behavior device plus two parameters; static asserts pin both limits to root plan 4.4.
    2. One output representation for characters, commands, and custom actions; no separate output types.
    3. `enum braille_word_position` carries the plan's `any` / `word-start` / `word-middle` / `word-end` constraint.
    4. UEB context follows `references/research/ueb-indicator-context.yaml`: `struct braille_context` with exactly `numeric`, `caps` (`none`/`word`/`passage`), and `word_start`; `BRAILLE_CONTEXT_INIT` is the reset state (numeric off, caps none, word_start true).
    5. Every rule carries an optional `struct braille_rule_when` tri-state predicate (`any`/`set`/`clear` per field; caps `set` means word or passage), one `enum braille_rule_class` naming the kind of sign (all 11 classes from the research transition table), and a `sets` capitals mode meaningful only on `BRAILLE_CLASS_CAPS_INDICATOR` rules.
    6. The word-start constraint is the `word_start` field, verified by the disjoint-⠦-rows test.
    7. `braille_rule_when_holds` is the header-inline predicate evaluator the matcher leaf will use; there is no per-character dispatch.
    8. Records are fixed-size, so read-only factory tables are `static const` arrays, the initially empty override layer is a mutable in-memory array, and later persistence serializes the same struct; `struct braille_rule_table` gives both one read-only view shape.
3. No matcher, dispatch, table registration, or firmware wiring was added; those are sibling leaves. `braille_engine.c` and the rule-table stubs are unchanged.

## 2. Host verification

1. `python3 tests/host/test_rules.py` compiles `tests/host/test_rules.c` with `-Wall -Wextra -Werror` (split and non-split) and runs it: schema limits, reset context, the full 27-predicate × 12-context `when` truth table, same-cells disjoint predicates, `sets` only on caps indicators, uniform bindings output, empty/mutable override view, and a maximum-size 8-chord/16-action rule. Exit 0.
2. `python3 tests/host/test_chord.py` still passes, exit 0.
3. `python3 -m unittest discover` in `tests/host` shows the same 11 failures + 2 errors as the clean tree (verified by stashing): pre-existing devicetree/kconfig/keyboards environment issues, untouched by this leaf.

## 3. Preserved unrelated work

1. The uncommitted `AGENTS.md` edit and the untracked message in `02-capture-chords-and-the-braille-mode-key/01-wire-braille-keys-to-a-thin-behavior-driver/` were left alone.

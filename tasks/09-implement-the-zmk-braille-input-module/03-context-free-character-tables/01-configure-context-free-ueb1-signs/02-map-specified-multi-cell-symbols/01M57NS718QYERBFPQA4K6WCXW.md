---
created: Mon 19 Oct 2026 - 01h 13m 51s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: symbols
subject: Specified multi-cell symbols delivered
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: gpt-6.1-sol
    name: symbols
---

# Results

1. Added all 21 specified ordered two-cell symbols to `src/braille_rules_grade1.c` using the existing static schema, matcher, and `&kp` dispatcher.
   1. Every row has any-context predicates; only `(`, `[`, and `{` carry opening-class metadata.
   2. No context transitions, deferred literary signs, or later tasks are implemented.
2. Recovered only the required multi-cell entries in `references/research/ueb-literary-signs.yaml` from `c786dd1`, checked against the existing unchanged authoritative ICEB PDF.
   1. Each row records its ordered dots, printed result, section, and printed page.
   2. Updated `references/README.md`, appended the implementation cheatsheet, and compiled its PDF.
   3. Did not modify or stage any `docs/braille-cheatsheet/` files or their removed generators.
3. Added `tests/host/test_symbols.c` and `test_symbols.py` plus the needed host keycode stubs.
   1. All 21 symbols exercise production press/release capture, table selection, matching and dispatch at reset and after a letter.
   2. Assertions cover silent pending prefixes, exact ordered cells and raw host usages including Shift, reversed order, a continuous stream, each first cell followed by non-continuing z, exact dropped-prefix notices, and fresh re-reading of the next chord.
   3. All four prefixes also recover before another symbol prefix and before every real phase-02 command; the real character/command tables validate together.
   4. The 21 existing YAML sign cases and seven scoped examples pass in split/non-split and optional Grade-2 builds, and with the pinned real ZMK keycode header.
4. Checks passed.
   1. `uv run --no-project --with pyyaml --python /opt/homebrew/bin/python3 -m unittest discover -s tests/host -p test_symbols.py -v`: four Python tests, five C configurations, all 28 scoped YAML cases per configuration, and every cited rulebook-page pattern.
   2. The explicit host regression set excluding the unrelated cheatsheet/coordinator suites passed 32 tests using isolated PyYAML and kconfiglib dependencies.
   3. `./scripts/smoke-build qbf-left toucan-left`: both central-half module, layout, link and UF2 checks passed.
   4. `./scripts/test-commands`: both real command tables, sticky modifiers/Globe, HID and timeout integration checks passed.
   5. `typst compile references/braille-cheatsheet.typ`, `git diff --check`, and TaskDown validation passed.
5. Residual risks and boundaries.
   1. Broad host discovery is not green: unchanged legacy cheatsheet tests target the old reference document and missing broader research, unchanged coordinator tests depend on an unavailable tmux pane, and the default environment lacks Python dependencies. These are outside this leaf; the scoped regression environment includes its required dependencies.
   2. Baseline builds retain existing deprecated-Kconfig and Toucan scroll-macro warnings; physical keyboard/VoiceOver acceptance remains user-owned.
   3. No implementation blocker remains; no push or hardware acceptance is claimed.

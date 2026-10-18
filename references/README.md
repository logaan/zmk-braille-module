# Normative UEB reference

The source of truth is [The Rules of Unified English Braille, Third Edition
2024](iceb/rules-of-unified-english-braille-2024.pdf), published by the
International Council on English Braille (ICEB), edited by Matthew Horspool.
Select uncontracted requirements using sections 2.5 and 5.11; the book covers
both grades.

## Provenance and license

- Publisher: <https://iceb.org/ueb.html>.
- Original download:
  <https://iceb.org/wp-content/uploads/2025/10/Rules-of-Unified-English-Braille-2024.pdf>.
- Retrieved: 2026-09-26; restored unchanged from repository commit `f3b5083`.
- Edition: Third Edition 2024; electronic print ISBN 978-1-7383661-0-1.
- SHA-256: `bf742499ee2eb3cbdf4914d97bbf0b2857bd3e91621f04a4990091b64e5a105e`.
- Copyright: Third Edition © 2024, International Council on English Braille.
- The copyright page (printed page ii, PDF page 2) licenses the publication
  under **Creative Commons Attribution–Noncommercial–No Derivative Works 2.5
  Australia**: <https://creativecommons.org/licenses/by-nc-nd/2.5/au/>.
- Preserve attribution and the license. Redistribute this PDF unchanged and
  only for noncommercial purposes under that license, independently of the
  module's code license. Do not bundle it into a commercial distribution under
  an assumed code license. Inclusion does not imply ICEB endorsement.
- The PDF is the authority. Local text extraction may damage Braille cells;
  do not commit modified derivatives of the reference.

## Implemented context-free core punctuation

Task 9.3.1.1 adds these unconditional, single-cell UEB1 rows to the existing
alphabet in `src/braille_rules_grade1.c`. Each invokes one ordinary ZMK `&kp`
press/release, not a Unicode Braille character. The host layout is US.

| Result | Dots | Mask | ZMK keycode | Rule class |
| --- | --- | --- | --- | --- |
| Comma `,` | 2 | `0x02` | `COMMA` | numeric-punctuation |
| Semicolon `;` | 2,3 | `0x06` | `SEMI` | other |
| Colon `:` | 2,5 | `0x12` | `COLON` (Shift+semicolon) | other |
| Exclamation `!` | 2,3,5 | `0x16` | `EXCL` (Shift+1) | other |
| Apostrophe `'` | 3 | `0x04` | `SQT` | opening |
| Hyphen `-` | 3,6 | `0x24` | `MINUS` | hyphen |
| Full stop `.` | 2,5,6 | `0x32` | `DOT` | numeric-punctuation |

The patterns are from section 7, printed page 75 (PDF page 103). Class
metadata preserves the existing context design from commit `2d6250d`:
section 6.2.1 (p.66) permits comma and full stop in numeric mode;
sections 2.6.1–2.6.2 (pp.15–16) distinguish hyphen boundaries and intervening
opening symbols including apostrophe. These classes do not implement any
context transitions in this leaf; phase 04 owns those transitions.

All seven rows have `any` predicates and no word-position restriction.
Dots 2,3,6 (`0x26`) deliberately remain unassigned: selecting question mark or
opening double quotation mark requires phase-04 word context (7.5 and 7.6).
The existing question/quotation acceptance cases in
`tests/braille/ueb1/punctuation.yaml` are retained for that later task.
Multi-cell symbols and other literary signs are not implemented by this leaf.

## Checks and build

- `python3 -m unittest discover -s tests/host -p test_punctuation.py -v`
  exercises all seven rows through production capture, table selection,
  matching, and dispatch against the real phase-02 command table, with split
  and optional Grade-2 builds. If the pinned ZMK workspace exists, it also
  checks the actual ZMK keycode header instead of just the host stub.
- The shared alphabet and command test suites cover their existing mappings
  with the expanded UEB1 table.
- `typst compile references/braille-cheatsheet.typ` builds the append-only
  implementation reference. Its generated PDF is ignored by Git.
- The separate `docs/braille-cheatsheet/` document is not modified here.
- Physical keyboard/VoiceOver acceptance remains user-owned.

## Implemented specified multi-cell symbols

Task 9.3.1.2 extends `src/braille_rules_grade1.c` with the 21 ordered two-cell
signs below. The table now contains 26 lowercase letters, seven core
punctuation signs, and these 21 symbols. Each complete sequence dispatches
one `&kp` press/release with the US-layout keycode, including implicit Shift
where needed. No prefix cell emits a key by itself.

| Result | Ordered masks | ZMK keycode |
| --- | --- | --- |
| `&` | `0x08` then `0x2f` | `AMPS` |
| `*` | `0x10` then `0x14` | `STAR` |
| `@` | `0x08` then `0x01` | `AT` |
| `^` | `0x08` then `0x22` | `CARET` |
| `#` | `0x38` then `0x39` | `HASH` |
| `$` | `0x08` then `0x0e` | `DLLR` |
| `%` | `0x28` then `0x34` | `PRCNT` |
| `~` | `0x08` then `0x14` | `TILDE` |
| `/` | `0x38` then `0x0c` | `FSLH` |
| `_` | `0x28` then `0x24` | `UNDER` |
| `+` | `0x10` then `0x16` | `PLUS` |
| `=` | `0x10` then `0x36` | `EQUAL` |
| `<` | `0x08` then `0x23` | `LT` |
| `>` | `0x08` then `0x1c` | `GT` |
| `(` | `0x10` then `0x23` | `LPAR` |
| `)` | `0x10` then `0x1c` | `RPAR` |
| `[` | `0x28` then `0x23` | `LBKT` |
| `]` | `0x28` then `0x1c` | `RBKT` |
| `{` | `0x38` then `0x23` | `LBRC` |
| `}` | `0x38` then `0x1c` | `RBRC` |
| `\` | `0x38` then `0x21` | `BSLH` |

Dots 4 (`0x08`), 5 (`0x10`), 4,6 (`0x28`), and 4,5,6 (`0x38`) are strict
prefixes held by the existing matcher without a timer. A non-continuation
drops only the incomplete prefix with the existing notification hook; the
following chord is read afresh. Thus dots 4 then 1 produce `@`, not `a`,
while dots 4 then 1,3,5,6 produce `z` and a dropped-prefix notice. Another
first cell can itself become a fresh pending prefix after such a drop.
Factory space, Tab, Return, navigation, Delete, and modifier chords still
run through their real phase-02 command table.

All 21 rules have `any` predicates and no word-position restriction. Only
`(`, `[`, and `{` carry the opening class (2.6.2, p.16); `<` is class other,
as are the remaining symbols. No context transition is added. Numeric,
capitals, Grade-1, and question/quotation policy remain phase-04 work.

Only the needed multi-cell-symbol section of the historical research was
recovered in [`research/ueb-literary-signs.yaml`](research/ueb-literary-signs.yaml).
It records all ordered dots and each sign's authoritative rulebook section
and printed page, checked against the unchanged PDF. It does not claim the
other historical literary-sign research or deferred signs are implemented.

`tests/host/test_symbols.py` requires PyYAML and a C compiler. Run it with
`python3 -m unittest discover -s tests/host -p test_symbols.py -v` in an
environment with PyYAML, or use `uv run --no-project --with pyyaml python -m
unittest discover -s tests/host -p test_symbols.py -v`. It covers:

- All 21 sequences through production key-edge capture, matching, and dispatch,
  including the silent pending first cell, correct press/release usages and
  Shift, reset/after-letter input, reversed order, and a continuous stream.
- Every sign's first cell followed by a non-continuing `z`, verifying the
  exact dropped-prefix notice and that `z` is emitted once and input recovers.
- All four prefixes followed by another prefix and by every real factory
  command; factory collision validation includes both real UEB1 tables.
- Split/non-split and optional Grade-2 build configurations, plus the real
  pinned ZMK keycode header when available.
- The 21 existing YAML sign fixtures and seven scoped examples without any
  premature implementation of their neighboring contextual acceptance cases.
- All patterns on their cited ICEB pages when Poppler's `pdftotext` is available.

The append-only implementation cheatsheet is rebuilt with Typst; the separate
`docs/braille-cheatsheet/` files are untouched. QBF and Toucan central-half
firmware smoke builds pass; physical keyboard/VoiceOver acceptance remains
user-owned.

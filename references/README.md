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

## Published Computer8 mapping and VoiceOver provenance

Task 9.3.2.1.1 publishes [`research/computer8.yaml`](research/computer8.yaml).
Its 16-by-16 matrix gives an explicit entry for every mask `0x00`–`0xff`:
94 printable non-space ASCII code points, two character-reserved command
positions (dot 7 Delete and dot 8 Return), and 160 reserved patterns including
unenterable zero. Matrix row/column are the mask's high/low nibbles. ASCII
space is a separate Braille-space-only chord, `0x100`.

The source is the **Liblouis U.S. English (EBAE) eight-dot table supported by
VoiceOver**, not a claim that every VoiceOver installation or user chooses
one universal US table. On macOS 26.6.2 (25G83), inspect:

```text
/System/Library/ScreenReader/BrailleTables/
  LiblouisBrailleTranslator.brailletable/Contents/Resources/
    Tables.plist
    LiblouisBrailleTables.plist
    liblouis/tables/en_US-comp8-ext.tbl
    liblouis/tables/en-us-comp8-ext.utb
    liblouis/tables/latinLetterDef8Dots.uti
    liblouis/tables/loweredDigits6Dots.uti
```

`Tables.plist` registers `eng-xna_8dot.table`, `eng-xna.8Dot`, and
`eng-xueb.8Dot` as `en_US-comp8-ext.tbl`. That wrapper includes
`en-us-comp8-ext.utb`; its 94 non-space ASCII definitions equal the original
project fixture source `en-us-comp8.ctb` and its letter/digit includes.
`LiblouisBrailleTables.plist` identifies Liblouis 3.36.0, commit
`b798a79ebc9ffe7f5fc56c09e87c75c17d3583e8`.
The research records installed file SHA-256 values and
[version-pinned upstream sources](https://github.com/liblouis/liblouis/tree/v3.36.0/tables).
The upstream character tables and includes match the installed bytes; only
the wrapper's metadata comments differ. No Apple bundle or upstream table
is redistributed; upstream tables retain their LGPL-2.1-or-later license.

Apple's [Translation settings documentation](https://support.apple.com/en-au/guide/voiceover/vo3017c92426/mac)
confirms separately selectable input/output tables and six/eight-dot input;
it does not publish the exhaustive ASCII mapping. The installed registration
and definitions supply that evidence. The legacy **Duxbury** US table
(`Duxbury.brailletable/Contents/Resources/en-US.8dot.plist`) differs: it puts
underscore on dots 4,5,6,7, not the selected Liblouis dots 4,5,6. Do not
silently mix those variants.

Dot n is bit n−1, including independent dot 7 (`0x40`) and dot 8 (`0x80`).
Uppercase letters use the lowercase pattern plus dot 7 in the *same* cell;
no prior indicator changes the next cell. No printable ASCII row uses dot 8.
Backtick is dot 4, vertical bar is dots 1,2,5,6, and underscore is dots 4,5,6.

The module intentionally takes only printable ASCII, **not** the source's
extended Unicode or control-character mappings. Nonzero reserved patterns,
including dots 7,8 together (`0xc0`), must emit no host output and invoke the
existing dropped-prefix notice hook; phase 05 supplies its brief unassigned
display presentation. They remain available to user overrides. Zero dots
is reserved but cannot form a chord, so it emits neither output nor notice.

[Apple's 8-dot command table](https://support.apple.com/en-us/103560) assigns
dot 7 alone to Delete and dot 8 alone to Return. Those stay exclusively in
`computer8`'s factory command table, not as duplicate character or no-output
rules. Space-bearing commands (including Tab `0x11e`) cannot collide with
dot-only character rows. No general command-over-character priority is
introduced: the existing factory collision validator must accept the pair.

`tests/host/test_computer8_mapping.py` checks all 256 entries, the 94 original
ASCII fixtures, all 159 enterable reserved fixtures, and independent dot-bit
policy. When the macOS provider exists it also compares the actual registered
US table and the original fixture table. Run with `uv run --no-project --with
pyyaml python -m unittest discover -s tests/host -p test_computer8_mapping.py -v`.
This publishes and verifies the mapping; firmware implementation and physical
acceptance are separate descendant tasks.

## Implemented Computer8 characters

Task 9.3.2.1.2 populates `src/braille_rules_computer8.c` with all 94
printable non-space ASCII cells using the existing behavior-action schema.
Each emits one US-layout `&kp` press/release; ZMK's implicit Shift encodes
uppercase and shifted symbols. All rows have any predicates/word position
and neutral class. No engine, capture, or dispatch special case is added.
Reserved cells remain absent from the factory rule array: the shared matcher
emits nothing and calls its existing notification hook, with no zero-action
rule to collide with commands. The phase-05 display is not implemented.

`tests/host/test_computer8.py` checks every mask through production capture,
selection, matching and dispatch against independent raw HID expectations
and the published matrix. It covers all original character/example and
isolation fixtures, all 159 enterable reserved fixtures, exact notices and
recovery, live override binding of each reserved cell, and a stream of all
94 printable non-space characters with no factory buffering. Split/non-split,
optional Grade-2 builds and the pinned real ZMK keycode header pass.

The native `scripts/test-commands` fixture now requires real character tables
instead of Computer8 override stand-ins. It checks representative letters,
case, digits, backtick, bar, underscore, shifted symbols, and reserved cells
through unmodified ZMK `kp`/HID, plus both command tables and real sticky-key/
Globe semantics and timeouts. Its native USB endpoint diagnostics are expected
because there is no physical USB endpoint; all asserted HID checkpoints pass.
Smoke builds of QBF left, Toucan left and Toucan with the Computer8 boot-mode
overlay pass. No device is flashed and no physical acceptance is claimed.

The full host-suite attempt also exposes pre-existing failures in
`test_cheatsheet.py` (stale document contract and removed research) and
`test_coordinator.py` (obsolete tmux mocks). Both failure sets reproduce from
starting commit `53e9439` in an isolated archive; they are not repaired by this
mapping task. Install `kconfiglib` as well as PyYAML for the Kconfig tests.

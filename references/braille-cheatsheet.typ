// Append-only implementation reference; add new information at the end.
#set page(paper: "a4", margin: 18mm)
#set text(font: ("DejaVu Sans Mono", "Apple Braille"), size: 10pt)

= Implemented UEB1 core punctuation

Task 9.3.1.1 adds seven context-free cells to the existing lowercase
alphabet. These rows emit ordinary US-layout host keystrokes through ZMK
`&kp`; they do not send Unicode Braille.

#table(
  columns: (auto, auto, auto, auto),
  [Result], [Dots], [Cell], [ZMK keycode],
  [Comma ,], [2], [⠂], [`COMMA`],
  [Semicolon ;], [2,3], [⠆], [`SEMI`],
  [Colon :], [2,5], [⠒], [`COLON` (Shift+semicolon)],
  [Exclamation !], [2,3,5], [⠖], [`EXCL` (Shift+1)],
  [Apostrophe '], [3], [⠄], [`SQT`],
  [Hyphen -], [3,6], [⠤], [`MINUS`],
  [Full stop .], [2,5,6], [⠲], [`DOT`],
)

Authority: ICEB, _The Rules of Unified English Braille_, third edition 2024,
section 7, printed page 75. The unchanged PDF and its independent reference
license are documented in `references/README.md`.

All seven rows have any-context predicates. Comma/full stop retain the
numeric-punctuation class (6.2.1); apostrophe retains the opening class
(2.6.2); hyphen retains its boundary class (2.6.1). Other rows use the other
class. Context transitions remain phase-04 work.

Dots 2,3,6 (⠦) are not assigned yet: question mark versus opening double
quotation mark requires word context. Multi-cell symbols and the remaining
literary signs are later tasks. No hardware acceptance is claimed.

== Verification boundary

The seven mappings are checked through production chord capture, matching,
and ordered dispatch with the real factory command table. Colon and
exclamation include the implicit Shift bit in the ZMK keycode; neither
requires a separate sticky modifier action. QBF and Toucan central-half
firmware builds pass, but host and physical-device acceptance are separate.

= Printable quick-reference presentation

The printable reference lives in `docs/braille-cheatsheet/`. Table bottom
borders now match their top and sides, including column continuations.
Braille columns and wrapped sequences are centered except in Capitals.
Capitals includes the Grade 1 symbol, word, and passage indicator examples;
its examples show UEB sequences only, while the short dot-7 rule remains.
The separate Capitalised contractions section is omitted. All 182
contractions and 22 keyboard chords remain. These are presentation changes,
not changes to implemented mappings.

= Implemented specified two-cell UEB1 symbols

Task 9.3.1.2 adds the following 21 symbols, with successive cells in the
listed order. Each complete sequence emits one ordinary US-layout ZMK
`&kp` press/release; shifted characters use the keycode's implicit Shift.

#table(
  columns: (auto, auto, auto, auto),
  [Result], [Dots in order], [Cells], [ZMK keycode],
  [Ampersand `&`], [4 then 1,2,3,4,6], [⠈⠯], [`AMPS`],
  [Asterisk `*`], [5 then 3,5], [⠐⠔], [`STAR`],
  [At `@`], [4 then 1], [⠈⠁], [`AT`],
  [Caret `^`], [4 then 2,6], [⠈⠢], [`CARET`],
  [Number sign `#`], [4,5,6 then 1,4,5,6], [⠸⠹], [`HASH`],
  [Dollar `$`], [4 then 2,3,4], [⠈⠎], [`DLLR`],
  [Percent `%`], [4,6 then 3,5,6], [⠨⠴], [`PRCNT`],
  [Tilde `~`], [4 then 3,5], [⠈⠔], [`TILDE`],
  [Forward slash `/`], [4,5,6 then 3,4], [⠸⠌], [`FSLH`],
  [Underscore `_`], [4,6 then 3,6], [⠨⠤], [`UNDER`],
  [Plus `+`], [5 then 2,3,5], [⠐⠖], [`PLUS`],
  [Equals `=`], [5 then 2,3,5,6], [⠐⠶], [`EQUAL`],
  [Less-than `<`], [4 then 1,2,6], [⠈⠣], [`LT`],
  [Greater-than `>`], [4 then 3,4,5], [⠈⠜], [`GT`],
  [Open parenthesis `(`], [5 then 1,2,6], [⠐⠣], [`LPAR`],
  [Close parenthesis `)`], [5 then 3,4,5], [⠐⠜], [`RPAR`],
  [Open square bracket `[`], [4,6 then 1,2,6], [⠨⠣], [`LBKT`],
  [Close square bracket `]`], [4,6 then 3,4,5], [⠨⠜], [`RBKT`],
  [Open curly brace `{`], [4,5,6 then 1,2,6], [⠸⠣], [`LBRC`],
  [Close curly brace `}`], [4,5,6 then 3,4,5], [⠸⠜], [`RBRC`],
  [Backslash `\`], [4,5,6 then 1,6], [⠸⠡], [`BSLH`],
)

Authority: ICEB 2024 sections 3 and 7. Per-sign sections and printed pages
are recovered in `references/research/ueb-literary-signs.yaml` from the
historical research and checked against the unchanged authoritative PDF.

The four first cells (4; 5; 4,6; 4,5,6) are silent strict prefixes, held
without a timer. A non-continuation drops the prefix with a notice and is
interpreted afresh: 4 then 1 emits `@`, but 4 then 1,3,5,6 drops 4 and emits
`z`. A new prefix after a drop can itself remain pending.

Every symbol has any-context predicates. Open parenthesis, square bracket,
and curly brace use the opening class (2.6.2); less-than and the other
symbols use the other class. These are metadata only: context transitions
and question/quotation policy remain phase-04 work. This supersedes the
earlier note that these 21 multi-cell symbols are still future work.

All 21 symbols and their first-cell non-continuations are tested through
production capture, matching, and dispatch, with real command-table
validation, all-command recovery, split/Grade-2 build variants, pinned ZMK
keycodes, 21 YAML sign fixtures and seven scoped examples. Both central-half
firmware smoke builds pass. The separate printable cheatsheet is untouched;
physical keyboard/VoiceOver acceptance remains user-owned.

= Printable quick-reference mnemonics and layout

The printable reference in `docs/braille-cheatsheet/` is split into modules:
`style.typ`, `data.typ`, `cells.typ`, `mnemonics.typ`, `tables.typ`, and
`validation.typ`, with `braille-cheatsheet.typ` as the entry point. The
second heading is now "UEB Grade 2".

Tables whose inputs are letters or letter-derived signs gain Output, Input,
and Mnemonic columns: Navigation and Editing, Alphabetic wordsigns, Lower
wordsigns, Lower groupsigns (all three), Initial-letter contractions,
Final-letter groupsigns, and Shortforms. Mnemonics are derived from the
actual cells: letter cells spell the letter, and the ten lowered a–j cells
spell ↓ plus the letter. Non-letter signs stay vector braille; rows without
any mnemonic show a dash. Strong wordsigns, Strong contractions, and Strong
groupsigns are labeled Output and Input without a mnemonic column; Sticky
modifiers is labeled Modifier and Input. Mapping data is unchanged: all 182
contractions, 22 keyboard chords, and every character cell are identical.
Compilation asserts the mnemonic spellings round-trip to the exact input
cells and that labeled tables have one column per header. The reference
still fits two A4 pages with no overlapping or out-of-bounds text.

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

= Printable quick-reference arrangement

The printable reference drops the "Characters & keyboard chords" heading.
Capitals follows Lowercase in the first page column; Digits occupies the
second column and Symbols holds the third column alone through explicit
weak column breaks, so the keyboard chords begin page 2 together. Missing
mnemonics leave their cell empty instead of a dash. Compilation now
requires all 22 chords to share one page rather than page 1. The reference
is three A4 pages with no overlapping or out-of-bounds text.

= Printable quick-reference underline convention

The dedicated Symbols column is reverted: Digits and Symbols share the
second page column, the keyboard chords flow onto page 1 again, and the
reference is two A4 pages. Chords held with Braille Space no longer show a
Space key and plus sign; their dot cell is underlined instead, in both the
Input and Mnemonic columns. The plain Space row keeps its key box, dot-only
chords stay unmarked, and missing mnemonics remain empty cells.

= Printable quick-reference chord merge

Sticky modifiers merge into the Navigation and Editing table, whose 22 rows
now end with the five modifiers. Each modifier name and input cell carries a
trailing underscore: "Shift _" with its underlined cell followed by "_",
marking that the chord holds its effect for the next input. The UEB Grade 2
heading is dropped; the Grade 2 sections follow the chord table directly.

= Printable quick-reference space row

The Lowercase table drops its Braille Space row and is retitled "Lowercase
letters"; the Space chord remains in Navigation and Editing. The character
coverage assertion now covers printable ASCII excluding space. The
reference stays two A4 pages with no overlapping or out-of-bounds text.

= Printable quick-reference digit tables

The Digits table splits into "Digits · UEB 1" and "Digits · 8 dot", each
with Number, Input, and Mnemonic columns. The 8-dot digit cells are the
lowered a–j letters, so their mnemonics use the down arrow: 0 is ↓j and 1
is ↓a. UEB 1 mnemonics keep the number sign before the letter. Modifier
outputs and inputs end with an ellipsis instead of an underscore, and
Space chords show the bordered Space key plus the dot cell again instead
of an underline. A page break puts Alphabetic wordsigns at the top of
page 2; the reference is three A4 pages with no overlapping or
out-of-bounds text. A multiline expected-row expression in the compilation
checks silently counted only its first line and is now parenthesized.

= Printable quick-reference symbol pairing

Symbols precedes the digit tables: Lowercase and Capitals fill the first
page column, Symbols fills the second, and a weak column break starts
Digits · UEB 1, Digits · 8 dot, and Navigation and Editing in the third.
Within Symbols, opening and closing pairs are adjacent: ( with ), < with >,
[ with ], and { with }, so = follows the angle pair, \ follows the bracket
pair, and | follows the brace pair. A compilation assert pins each closing
symbol to the row after its opening counterpart.

= Printable quick-reference thinner cell rules

Every cheatsheet table's internal row separators drop from 0.3pt to 0.2pt
while the 1pt black top, side, and bottom frame rules and the 0.4pt Space
key outlines stay unchanged. This is a presentation change only.

= Printable quick-reference space symbol

Input and Mnemonic columns replace the word Space inside the bordered key
box with the open-box symbol ␣ (U+2423). The Navigation and Editing Space
chord keeps its name Space in the Output column, and Capitals examples
show ␣ wherever their braille sequences contain a space. Rendering and
validation are unchanged; this is a presentation change.

= Printable quick-reference sticky ellipsis spacing

Sticky modifier outputs drop the space before the ellipsis: Shift…,
Control…, Option…, Command…, and Globe… now have no space before the
ellipsis. The compilation assert matches the new form.

= Printable quick-reference capitals table

Capitals becomes a two-column table with Output and Input headers. Each
row pairs the printed example with its complete UEB 1 sequence: A with
⠠⠁, ABC with ⠠⠠⠁⠃⠉, ABc with ⠠⠠⠁⠃⠠⠄⠉, A BIG CAT with ⠠⠠⠠⠁ ⠃⠊⠛ ⠉⠁⠞⠠⠄,
b with ⠰⠃, cd with ⠰⠰⠉⠙, and b c d with ⠰⠰⠰⠃ ⠉ ⠙⠰⠄. The per-row scope
headings (Letter prefix, Word prefix, and so on) and the UEB 1 mode
labels are dropped; the dot-7 intro line stays. Sequences that exceed
the Input column wrap inside their cells, and spaces in those sequences
show the open-box symbol. The mode-example and capital-example helpers
are removed. Capitals still renders exactly seven rows, checked by the
compilation assertion.

= Printable quick-reference continuous columns

The weak column break after Symbols is dropped. Lowercase, Capitals,
Symbols, both digit tables, and Navigation and Editing now flow
continuously through page 1's three columns: Symbols continues at the
top of column 2 as "Symbols Cont'd", and Navigation and Editing starts
at the bottom of column 2 and continues at the top of column 3 as
"Navigation and Editing Cont'd". All chords stay on page 1, which the
compilation assert checks. The reference remains three A4 pages with
every continued heading starting its page column.

= Printable quick-reference digits grade 1 rename

The first digit table's heading changes from "Digits · UEB 1" to
"Digits · UEB Grade 1". The data module and the 8 dot digit table are
unchanged.

= Printable quick-reference sequence space parameter

The cell renderer factors the bordered key box out of the space-key
helper into a general key-box function, and defines the space key as
key-box[␣]. The sequence renderer gains a space parameter: auto (the
default) keeps the bordered ␣ key box between words, while any other
content — such as a fixed h(6pt) gap — replaces it. Tables keep the
default.

= Printable quick-reference Quick Braille Fox subheads

Each Quick Braille Fox heading gains a centred three-column grid
subhead: the plain text "qk brl fox", a middle dot, and the sequence
⠟⠅ ⠃⠗⠇ ⠋⠕⠭ rendered by the sequence cell renderer with space:
h(6pt) so word gaps are plain horizontal space instead of key boxes.
Rendering the cells through the sequence SVG renderer — rather than as
Unicode braille text — also fixes the cells showing question-mark
glyphs. A linebreak before each heading keeps it off the previous
content's line, and the headings are coloured: teal on page 1 and blue
on page 2, matching the page section headings.

= Printable quick-reference capitals key-box intro

The Capitals intro sets "8 Dot" inside the white bordered key box and
capitalises the instruction: "Add dot 7 to each uppercase letter."

= Printable quick-reference Quick Braille Fox code label

The Quick Braille Fox subheads drop the middle-dot separator and set
"qk brl fox" as inline raw (monospace) code. The subhead grid becomes
two columns: the code label and the braille cell sequence.

= Printable quick-reference Quick Braille Fox key-box label

The Quick Braille Fox subheads set the monospace "qk brl fox" label
inside the same white bordered key box used for "8 Dot" and the space
key.

= Printable quick-reference scalable key box

The key-box helper gains a size parameter (default 7pt) that scales the
text and the box padding together. The Quick Braille Fox subhead labels
use 10pt so the boxed "qk brl fox" matches the height of the braille
cells beside it; "8 Dot" and the space key keep the default 7pt.

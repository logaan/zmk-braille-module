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

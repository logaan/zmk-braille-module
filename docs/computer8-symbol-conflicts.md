# Computer8 shorthand: single-cell symbol review

The blanket single-cell exclusion has been removed. **143 contraction entries
are enabled:** all 120 multi-cell contractions and 23 alphabetic wordsigns.
All 26 lowercase letter definitions agree between Computer8 and UEB fallback;
using a letter as a standing-alone wordsign is normal contextual shorthand.
For example, `b` can expand to `but`, while `b` within an ordinary word remains
a letter. This is not a conflict between base letter definitions.

**39 entries remain excluded**, covering **27 distinct cells**. They use
cells that Computer8 assigns to digits or punctuation, rather than shared
letters. Keeping the Computer8 symbol is the conservative policy pending
review. The table records exact character output, not a claim that every
human-language interpretation is unrelated. In particular, `&` and `and`
express related meaning, but replacing a literal ampersand changes the host
text and can break code or identifiers. That case remains explicit for review.

The 71 multi-cell entries involving Computer8 symbol cells are still enabled.
Their contextual sequence expansion is part of the requested shorthand mode;
they are not candidates for removal merely because the literal sequence differs.
Ordinary Computer8 mode remains available for literal input.

## Enabled symbol-sequence ambiguities

Keeping the 71 symbol-bearing multi-cell entries does not make their prefix
meanings identical to Computer8. They remain enabled by the requested policy,
with their genuine literal-sequence ambiguities documented for review. Examples:

| Dots | Literal Computer8 sequence | Shorthand expansion |
| --- | --- | --- |
| 5 / 145 | `"d` | `day` |
| 5 / 13456 | `"y` | `young` |

These longer matches take precedence over a single-letter wordsign. For
example, an opening quote followed by the `d` cell can match `day` rather than
be interpreted as an opening quote followed by the wordsign `do`. To enter
literal text containing such sequences, select ordinary Computer8 **before**
starting the word; changing mode discards the current pending buffer. All 71
entries are marked `enabled_symbol_sequence_ambiguity` in the exhaustive JSON.

## Excluded single-cell entries

Repeated cells appear where UEB2 gives a wordsign and a groupsign distinct
conditions. Counts are contraction entries, not distinct dot patterns.
`WORD` and `LOWER_WORD` require standing-alone word context; `ANY` allows a
groupsign; `START` and `MIDDLE` restrict its position within a word. These are
the source table's condition names, whose detailed UEB constraints remain in
the matcher. No excluded entry changes those constraints in UEB2 mode.

| Dots | Computer8 output | UEB2 contraction | UEB2 condition | Review reason |
| --- | --- | --- | --- | --- |
| 16 | <code>*</code> | child | WORD | Different punctuation/symbol |
| 1256 | <code>&#124;</code> | out | WORD | Different punctuation/symbol |
| 146 | <code>%</code> | shall | WORD | Different punctuation/symbol |
| 34 | <code>/</code> | still | WORD | Different punctuation/symbol |
| 1456 | <code>?</code> | this | WORD | Different punctuation/symbol |
| 156 | <code>:</code> | which | WORD | Different punctuation/symbol |
| 23 | <code>2</code> | be | LOWER_WORD | Digit |
| 26 | <code>5</code> | enough | LOWER_WORD | Digit |
| 236 | <code>8</code> | his | LOWER_WORD | Digit |
| 35 | <code>9</code> | in | LOWER_WORD | Digit |
| 356 | <code>0</code> | was | LOWER_WORD | Digit |
| 2356 | <code>7</code> | were | LOWER_WORD | Digit |
| 12346 | <code>&amp;</code> | and | ANY | Related meaning; distinct literal symbol |
| 123456 | <code>=</code> | for | ANY | Different punctuation/symbol |
| 12356 | <code>(</code> | of | ANY | Different punctuation/symbol |
| 2346 | <code>!</code> | the | ANY | Different punctuation/symbol |
| 23456 | <code>)</code> | with | ANY | Different punctuation/symbol |
| 345 | <code>&gt;</code> | ar | ANY | Different punctuation/symbol |
| 16 | <code>*</code> | ch | ANY | Different punctuation/symbol |
| 1246 | <code>$</code> | ed | ANY | Different punctuation/symbol |
| 12456 | <code>}</code> | er | ANY | Different punctuation/symbol |
| 126 | <code>&lt;</code> | gh | ANY | Different punctuation/symbol |
| 346 | <code>+</code> | ing | ANY | Different punctuation/symbol |
| 1256 | <code>&#124;</code> | ou | ANY | Different punctuation/symbol |
| 246 | <code>{</code> | ow | ANY | Different punctuation/symbol |
| 146 | <code>%</code> | sh | ANY | Different punctuation/symbol |
| 34 | <code>/</code> | st | ANY | Different punctuation/symbol |
| 1456 | <code>?</code> | th | ANY | Different punctuation/symbol |
| 156 | <code>:</code> | wh | ANY | Different punctuation/symbol |
| 23 | <code>2</code> | be | START | Digit |
| 25 | <code>3</code> | con | START | Digit |
| 256 | <code>4</code> | dis | START | Digit |
| 23 | <code>2</code> | bb | MIDDLE | Digit |
| 25 | <code>3</code> | cc | MIDDLE | Digit |
| 2 | <code>1</code> | ea | MIDDLE | Digit |
| 235 | <code>6</code> | ff | MIDDLE | Digit |
| 2356 | <code>7</code> | gg | MIDDLE | Digit |
| 26 | <code>5</code> | en | ANY | Digit |
| 35 | <code>9</code> | in | ANY | Digit |

## Indicators and commands

UEB indicators are also distinct from their Computer8 literal symbols:

| Dots | UEB use | Computer8 output |
| --- | --- | --- |
| 6 | Capital indicator | `,` |
| 3456 | Numeric indicator | `#` |
| 56 | Grade-1 indicator | `;` |

The hybrid keeps these Computer8 characters; it does not enable UEB indicator
state. Dot 7 Delete, dot 8 Return, and space-bearing commands remain intact.

## Evidence

The [machine-readable audit](../references/research/computer8-shorthand-conflicts.json)
contains all 182 entries, the 26 checked shared letter patterns, per-entry
choices, UEB source sections, and source hashes. Reproduce it with
`python3 scripts/audit-computer8-shorthand.py`. The exact Computer8 variant is
the project's recorded VoiceOver Liblouis US mapping, not every eight-dot
braille system. The contraction source is the repository's ICEB 2024 table.

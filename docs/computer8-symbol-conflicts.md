# Computer8 shorthand: category and symbol policy

Only **98 contraction entries are enabled** in 8-dot shorthand: **23 alphabetic
wordsigns** and **75 shortforms**. All 26 lowercase letter definitions agree
between Computer8 and UEB fallback. A shared letter can expand when standing
alone, such as `b` becoming `but`, while remaining a letter within a word.

**84 entries are excluded:** the 39 single-cell symbol contractions listed
below, all 33 initial-letter contractions, and all 12 final-letter groupsigns.
These excluded categories retain their literal Computer8 cells in this mode;
ordinary UEB2 continues to support its full contraction set.

The user selected the Computer8 meaning for every conflicting single-cell
symbol and indicator pattern. The 39 single-cell entries cover 27 distinct
cells. In particular, `&` and `and` have related meaning but different host
text; this mode retains the ampersand for that single cell.

## Retained shortform ambiguities

Of the 75 shortforms, **49 contain only shared letter cells** and **26 include
Computer8 symbol cells**. Their expansion remains intentional under the selected
shortform category. For example:

| Dots | Literal Computer8 sequence | Shortform expansion |
| --- | --- | --- |
| 23 / 14 | `2c` | `because` |
| 124 / 34 | `f/` | `first` |
| 16 / 1345 | `*n` | `children` |

To enter literal text containing such shortform spellings, select ordinary
Computer8 before starting the word; changing mode discards the pending buffer.
The removed initial-letter contractions no longer expand `"d` to `day` or
`"y` to `young`. Alphabetic wordsigns can still apply in eligible quoted word
contexts, so literal entry should use ordinary Computer8 when needed.

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

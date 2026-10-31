# Contracted UEB input

Enable `CONFIG_ZMK_BRAILLE_GRADE2=y` to register `ueb2`. It uses the common
capture, matcher, behavior dispatcher, command table, override layer and mode
selector. `ueb1` and `computer8` remain available without this option.

The engine collects a word until Braille space, Return or Tab, translates it,
then emits the whitespace. Punctuation stays buffered. Other command chords,
mode changes, layer changes, non-Braille input and settings entry discard it.
The 32nd cell resolves immediately at a synthetic word boundary; the next cell
starts a new word. Consequently, the boundary can change a contraction's
reading inside a word longer than 32 cells. There is no timer.

The mapping contains all 182 contraction entries from ICEB's *Rules of Unified
English Braille*, third edition (2024), sections 10.1–10.9. Appendix 1's 328
longer-word entries are lexical data, with bounded iterative matching rather
than expanded oversized rules. Grade 1 symbol, word and passage indicators,
numeric-induced Grade 1, capitalization, positional groupsigns, shortform
exceptions and uncontracted fallback participate in reading. Formatting
indicators permit textual reading but do not change host font styling.

The display preview evaluates pending cells plus the currently held chord
without committing anything. It returns the candidate containing the held
cell: for example `a` then held `b` previews `about`; held `c` instead previews
`according`. Once `ab` is buffered, held `c` previews the final `c` contribution.
The separate pending display retains the whole buffered input. Provisional
readings can change as more cells arrive.

`tests/host/test_grade2.py` runs the actual C capture/matching/dispatch path.
It covers every mapping in lowercase, initial capitals, capitals-word and
Grade 1 fallback; constructed Appendix 1 spellings; independent ICEB sentence,
indicator and forbidden-contraction examples; overrides; held-key previews;
and buffer commit/discard boundaries. Mapping tests are generated from the
mapping record and are not independent proof of every rulebook interpretation.
Physical keyboard, display and VoiceOver acceptance remains outstanding.

The original three Grade 2 research files mentioned by the old tasks were
absent from this checkout. The mapping and Appendix 1 facts were recovered
from the local authoritative PDF and the existing cheatsheet. Existing
cheatsheet data and layout did not change.

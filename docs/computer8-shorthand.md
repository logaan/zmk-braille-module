# Eight-dot shorthand

`computer8_short` is a separate, optional mode displayed as **8dot+short**
(**8SHR** on the narrow OLED). It is a project-defined hybrid: Computer8 cells
plus UEB2 shortforms and contextual alphabetic wordsigns. It is not an assertion that
standard eight-dot computer Braille uses UEB contractions.

Enable both `CONFIG_ZMK_BRAILLE_GRADE2=y` and
`CONFIG_ZMK_BRAILLE_COMPUTER8_SHORT=y`. Select it with
`&bselect BRAILLE_MODE_COMPUTER8_SHORT` (stable ID 4), the existing NEXT selector
(ID 3), or the settings mode picker (`s`). The selected mode is saved by name;
firmware without this optional mode safely falls back to its configured default.
Toucan provides a direct selector at Braille-layer position 18 (the base H key).

## Typing and preview

Like UEB2, this mode buffers up to 32 cells. Braille space, Return and Tab
resolve the word and then emit their normal command. Reaching 32 cells resolves
a synthetic word boundary. Other commands discard pending text; changing
mode, layer, entering settings or pressing a conventional key also clears it.
The dashboard previews held cells together with the pending word, without
sending output. This preview remains provisional until a boundary is entered.

For example, dots 1 followed by dots 12 represent `ab` in Computer8. In this
mode they preview and commit `about` when standing alone. A single dot 1 still
means `a`. Shared letter cells also act as UEB2 wordsigns when standing alone:
`b` becomes `but`, and `y` (dots 13456) becomes `you`. Within an ordinary word
they remain letters. The provisional preview follows these contextual rules. Unmatched eight-dot capital, digit and punctuation cells retain Computer8
meaning; matched multi-cell sequences can include punctuation cells. Capitals are not case-folded into contraction spellings: literal
uppercase `AB` stays `AB`. UEB capital, numeric and Grade 1 indicators are not
interpreted here; their cells retain their Computer8 characters.

To type an ambiguous literal sequence such as `ab`, switch to ordinary
`computer8` **before starting the word**. Switching after entering the cells
discards the pending buffer. Ordinary Computer8 continues to emit its cells
immediately. User overrides remain shared across modes and take precedence,
including overrides containing command or whitespace cells.

## Compatibility and review

All 26 lowercase letter patterns have the same base meaning in the project's
Computer8 table and UEB fallback. Shared definitions are compatible.

The [audit](../references/research/computer8-shorthand-conflicts.json) compares
182 contraction entries with their literal Computer8 spellings. A different
expanded output does **not** by itself establish a conflict in a shorthand
mode: `ab` becoming `about` is the intended feature. The earlier TaskDown
statement that all 182 entries conflict used an overly strict literal-preservation
criterion and is superseded by the clarified review.

The mode enables exactly two categories: **23 alphabetic wordsigns** and
**75 shortforms**, for **98 contraction entries**. Initial-letter contractions,
final-letter groupsigns, strong wordsigns/contractions/groupsigns and all lower
wordsigns/groupsigns are excluded. This supersedes the earlier 143-entry policy.

For example, `ab` still expands to `about`, while the former initial-letter
sequence dots 5 / 145 no longer expands to `day`; the quote remains
Computer8 punctuation and `d` follows the alphabetic wordsign rules. Shortforms themselves can contain symbol cells, such as
`al?` for `although`; their accepted contextual expansions remain enabled.
The exact excluded symbol definitions and category policy are in the
[symbol report](computer8-symbol-conflicts.md).

To type a literal standalone letter such as `b` or `y`, select ordinary
Computer8 before starting the word. No UEB Grade 1 escape is introduced in
this hybrid; its indicator cell remains Computer8 punctuation.

Ordinary fallback and commands come from Computer8. Word and punctuation
boundaries use Computer8 ASCII meaning. Appendix 1 longer words and the
supported productive shortform extensions reuse the UEB2 dictionary rules.

The original UEB2 mode retains all its own indicators and contractions. The
existing cheatsheet's Computer8 mapping and UEB2 tables remain reference data
for those respective modes; they are not a claim that every UEB2 entry applies
to this hybrid. Software and firmware evidence is in
[verification](verification.md). Physical typing, pairing and display acceptance
still require the user's boards.

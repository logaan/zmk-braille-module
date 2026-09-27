---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: UEB Grade 2 extension
cargo:
  type: task
  status: draft
---

# UEB Grade 2 extension

1. Add `ueb2` as an optional named mode after initial-feature acceptance; nothing here blocks phases 01–06.
2. Its table contains every contraction the rulebook defines, read as the rulebook's reading rules permit, including word-position and word-part restrictions.
    1. The 182 contractions fall into nine classes: alphabetic wordsigns, strong wordsigns, strong contractions, strong groupsigns, lower wordsigns, lower groupsigns, initial-letter contractions, final-letter groupsigns, and shortforms.
    2. Shortforms also apply in the longer words of the rulebook's Shortforms List and under its rules 10.9.3–10.9.5.
    3. `references/research/ueb-grade2-contractions.yaml` lists every contraction with its dot pattern, rulebook source, and a fixture.
3. `ueb2` buffers each whole word, up to 32 cells, and resolves it only on whitespace; the display shows the buffered cells, and uncontracted output is used where no valid contraction applies.
4. Reuse chord detection, common rules and actions, commands, overrides, mode selection, and the pending-cell display; add only contraction data and the word and word-part context it needs.

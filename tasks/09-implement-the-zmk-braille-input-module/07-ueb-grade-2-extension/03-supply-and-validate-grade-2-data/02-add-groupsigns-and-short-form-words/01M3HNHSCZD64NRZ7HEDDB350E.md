---
created: Mon 28 Sep 2026 - 00h 50m 49s
author: user
subject: Add groupsigns and short-form words
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Add groupsigns and short-form words

1. Add these classes from `references/research/ueb-grade2-contractions.yaml` to `braille_rules_grade2.c`, each under its class's reading rules:
    1. Strong groupsigns (12): their letters wherever they occur; the six that share a cell with a strong wordsign are groupsigns unless standing alone.
    2. Lower groupsigns (10): be, con, and dis only at the beginning of a word before a letter or contraction; ea, bb, cc, ff, and gg only between letters or contractions; en and in anywhere, with a standing-alone ⠢ read as enough.
    3. Final-letter groupsigns (12): only after a letter or contraction.
    4. Shortforms (75): standing alone, or inside a longer standing-alone word that qualifies under rules 10.9.2–10.9.5.
2. Keep the Shortforms List's 328 longer words and the rule 10.9.3 conditions as data for the shortform eligibility check; they are not rules of their own, because some need more than 8 cells.
3. Where a lower groupsign's condition does not hold, the cell keeps its `ueb1` punctuation meaning.
4. Emit contraction letters with the same capitalization as the other classes, and give each rule a source comment naming its rulebook section.
5. Unit-test the added rules alongside the data: every entry against its fixture under `fixtures.by_class`, each condition with an input where it holds and one where the punctuation or letter meaning applies, the Shortforms List and rule 10.9.3 cases, and the abouts, almosts, and hims exceptions.

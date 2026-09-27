---
created: Mon 28 Sep 2026 - 00h 50m 49s
author: user
subject: Add wordsigns and strong contractions
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Add wordsigns and strong contractions

1. Create `braille_rules_grade2.c` with these classes from `references/research/ueb-grade2-contractions.yaml`, each under its class's reading rules:
    1. Alphabetic wordsigns (23): the word only when standing alone, including the apostrophe forms of rule 10.1.2; otherwise the letter.
    2. Strong wordsigns (6): the word only when standing alone, including the apostrophe forms of rule 10.2.2; otherwise the strong groupsign.
    3. Strong contractions (5): their letters wherever they occur.
    4. Lower wordsigns (6): be, were, his, and was standing alone and not in contact with lower punctuation; enough standing alone; in wherever the word occurs.
    5. Initial-letter contractions (33): their word wherever the two cells occur.
2. Emit each contraction's letters as host keystrokes: lowercase; only the first letter uppercase after the capital prefix; every letter uppercase in a capitalised word or passage.
3. Give each rule a source comment naming its rulebook section.
4. Unit-test the added rules alongside the data: every entry against its fixture under `fixtures.by_class`, and each reading condition with one input where it holds and one where the cell keeps its letter or punctuation meaning.

---
created: Mon 28 Sep 2026 - 04h 48m 26s
author:
  type: agent
  harness: claude-code
  model: claude
subject: Standing alone, word start, and letter-neighbour conditions
cargo:
  type: task
  status: done
---

# Result

1. Summary
    1. A word is a symbols-sequence between spaces; a letters-sequence is a run of alphabetic signs, and letters and contractions are both alphabetic (2.1).
    2. Standing alone: bounded by a space, hyphen, or dash, with only the 2.6.2 opening and 2.6.3 closing punctuation and indicators between; 2.6.4 adds apostrophe words (it'd, could've) and words with interior capitals that meet 10.9.4.
    3. The beginning of a word follows a space, hyphen, or dash, allowing the 2.6.2 symbols; this is the `word_start` field.
    4. Word parts: be, con, and dis need a following letter or contraction; ea, bb, cc, ff, and gg need one on both sides; final-letter groupsigns need one before; an adjacent capitals indicator or terminator breaks each condition.
    5. be, were, his, and was are wordsigns only when standing alone and not touching lower punctuation, hyphen, dash, or any quotation mark (10.5.1); this is why ⠦⠲⠲⠲ reads as a quote and three full stops.
    6. Shortforms apply standing alone, with s or 's, in Shortforms List words, or under 10.9.3–10.9.4.
    7. Bridging, preference, pronunciation, and word-division rules are writing rules; they never change how a written sign reads.
    8. Every condition is decided inside one symbols-sequence, so resolving the buffered word on whitespace gives the back-translator all the context it needs.
    9. 17 ambiguous cell groups are recorded with the deciding rule and fixtures on each side.
2. Detail: `references/research/ueb-grade2-word-boundaries.yaml`.
3. Tasks updated
    1. `07-.../02-extend-common-context-and-buffering`: reused fields and the added conditions.
    2. `07-.../02-.../01-match-positional-grade-2-rules`: conditions, whole-word procedure, whitespace and 32-cell boundaries.
    3. `07-.../03-supply-and-validate-grade-2-data`: reading rules from the boundaries file.
    4. `07-.../03-.../03-test-iceb-contracted-and-fallback-examples`: fixtures from both files.

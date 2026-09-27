---
created: Mon 28 Sep 2026 - 04h 06m 39s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: Needed for a-j after digits and word-initial question marks
cargo:
  type: task
  status: done
---

1. In uncontracted text, grade 1 indicators are required only before a-j after a digit, comma or full stop (6.5.2) and before a question mark that would begin a word or stand alone (7.5.2-7.5.4); 5.11.1 names both.
2. Readers may write them anywhere (5.10.1); ueb1 accepts all four (⠰, ⠰⠰, ⠰⠰⠰, ⠰⠄) with no output.
3. Effects: each ends numeric mode (6.3.1) and a capitalised word (8.4.2 with 2.1), and the next sign does not begin a word (2.6.2), so ⠦ after one is a question mark (7.6.9).
4. They precede capitals indicators (5.8.1) and do not replace the numeric indicator (5.4.2, 6.5.2).
5. Firmware choice: grade 1 mode itself is not stored in the initial modes, because it changes no uncontracted output.
6. Record: `references/research/ueb-grade-1-indicators.yaml`, with 56 rulebook fixtures and 5 constructed fixtures.
7. Tasks updated: implement capitalization and grade 1 indicators, test Grade 1 against ICEB examples.

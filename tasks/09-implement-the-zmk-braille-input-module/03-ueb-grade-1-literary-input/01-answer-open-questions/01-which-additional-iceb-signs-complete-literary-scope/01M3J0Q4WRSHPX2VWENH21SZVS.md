---
created: Mon 28 Sep 2026 - 04h 05m 59s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: Quotes, vertical line and grave accent alone complete ueb1
cargo:
  type: task
  status: done
---

1. Rulebook signs added to `ueb1`: the directional, two-cell and nondirectional quotation marks (7.6), the vertical line ⠸⠳ (15.1, Appendix 3) and the grave accent alone ⠨⠡ (Appendix 3). With these, every printable US keyboard character has a UEB sign.
2. Dots 2,3,6 is an opening double quotation mark at the start of a word (2.6.2, 7.6.7) and a question mark elsewhere; ⠰⠦ is always a question mark (7.5.4, 7.6.9).
3. All dot patterns already in the tasks were verified against the rulebook; none were wrong.
4. Firmware choice: every double quotation form emits `"` and every single form emits `'`, because the target is the US keyboard.
5. Record: `references/research/ueb-literary-signs.yaml`, with 33 rulebook fixtures and 8 constructed fixtures.
6. Tasks updated: map the eight core punctuation signs, implement specified multi-cell symbols, implement other ICEB literary signs, check Grade 1 MacBook character coverage.

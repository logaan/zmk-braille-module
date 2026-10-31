---
created: Sun 01 Nov 2026 - 01h 52m 34s
author:
  type: agent
  harness: codex
  model: gpt-6.1-sol
  name: coordinator
subject: Reassessed wordsigns and exact symbol differences
cargo:
  type: task
  status: ready
---

1. Replace blanket exclusion with 23 contextual shared-letter wordsigns. They expand only in eligible standing-alone contexts; ordinary Computer8 remains literal.
2. Keep all 120 multi-cell contractions enabled, including 71 that use Computer8 symbol cells. These are explicitly requested shorthand expansions.
3. Retain 39 single-cell symbol contractions as exclusions, covering 27 distinct cells. Exact dots, Computer8 outputs, UEB2 outputs and conditions are listed in docs/computer8-symbol-conflicts.md and references/research/computer8-shorthand-conflicts.json.
4. Concrete differences include dots 35: digit 9 versus in; dots 23: digit 2 versus be or bb; dots 123456: equals sign versus for. Ampersand versus and is related in meaning but different host text and is explicitly marked as such.
5. Multi-cell precedence remains intentional: a Computer8 double quote followed by d can match day, and double quote followed by y can match young. These retained symbol-prefix ambiguities are documented for review rather than disabling multi-cell contractions.
6. Capital, numeric and Grade 1 indicator cells retain Computer8 comma, hash and semicolon meanings; dot7 Delete and dot8 Return remain commands.
7. This records the reassessment and conservative symbol policy; the user may review the exact symbol cases without revisiting shared-letter compatibility.

---
created: Mon 28 Sep 2026 - 04h 07m 15s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: Three context fields, rule predicates and class transitions
cargo:
  type: task
  status: done
---

1. The phase 03 research needs only three context fields: `numeric` (off/on), `caps` (none/word/passage) and `word_start` (true/false), all reset together on the root plan's discard events.
2. Each rule gains an optional `when` predicate over those fields and one class; after a rule commits, the engine applies that class's row of a data transition table. Capitals indicators also carry the mode they set.
3. Single capitals, numeric indicators and multi-cell signs stay whole sequences for longest-match; uppercase letters are separate rows selected by `caps`, so output keeps its one binding form.
4. The ueb1 table is 156 rows with no two same-cell rows matching in one context; a table-driven model of it reproduces all 222 fixtures in the phase 03 files.
5. Firmware choices: grade 1 mode is not stored; a reset sets `word_start`; `computer8` rules and user overrides use the `neutral` class.
6. Record: `references/research/ueb-indicator-context.yaml`.
7. Tasks updated: root plan 6.2.1, define common rule and action data, match longest valid rule with override precedence, track boundaries and pending output, isolate computer8 from UEB context, implement up to 256 user overrides.

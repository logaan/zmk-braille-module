---
created: Sun 18 Oct 2026 - 19h 09m 10s
author:
  type: agent
  harness: pi
  model: gpt-5.6-terra
  name: task-reorg
subject: Context-free character tables
cargo:
  type: task
  status: draft
---

# Context-free character tables

## 1. Purpose

1.  Configure character rules whose output is determined entirely by their
    one-to-eight-cell input sequence.
2.  Use the already delivered longest-prefix matcher, static table registry,
    ordered behavior actions, and dropped-prefix notification hook.

## 2. Promoted work

1.  Add unconditional UEB1 punctuation and the specified multi-cell keyboard
    symbols as factory rule data.
2.  Populate the stateless `computer8` table, including its explicit reserved
    patterns and its independently selected command table.
3.  Verify the resulting tables and test them on the baseline hardware.

## 3. Boundary

1.  A sign belongs here only when it has an `any` context predicate and no
    state transition is needed to choose its host output.
2.  The `computer8` table is stateless by design; it must stay isolated from
    UEB numeric, capital, word-position, and contraction semantics.
3.  The direct factory commands in phase 02 precede `computer8` so collisions,
    including dot-7 and dot-8 choices, are resolved against real command data.

## 4. Deferred work

1.  Numeric mode, capitalization, Grade 1 indicators, word-boundary state, and
    any context-dependent quotation/question-mark choice remain in phase 04.
2.  Settings, persistence, and a visual presentation of pending or unassigned
    cells remain in phase 05; the existing notification hook may be asserted by
    tests before a display implementation exists.

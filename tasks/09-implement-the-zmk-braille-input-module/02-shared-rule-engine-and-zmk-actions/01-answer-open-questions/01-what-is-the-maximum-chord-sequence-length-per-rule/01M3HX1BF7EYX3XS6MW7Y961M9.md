---
created: Mon 28 Sep 2026 - 03h 01m 39s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: 'Decided: 8 cells per rule'
cargo:
  type: task
  status: delivered
---

# Decision: 8 cells per rule

## 1. Decision

1. A rule matches at most 8 chords.
2. The same bound applies to factory rules, user overrides, settings capture, and persistence.

## 2. Rationale

1. Grade 1 needs up to 3 cells, e.g. the capitals passage and grade 1 passage indicators.
2. Optional Grade 2 short forms need up to 5, e.g. "perceiving".
3. The rest is margin for user sequences; each chord costs about 2 bytes.

## 3. Impact

1. `braille_rules.h` sequence arrays, the pending-cell buffer, capture, and the persisted format all use 8.
2. Pending input can never exceed 8 cells, since anything longer is always ruled out.

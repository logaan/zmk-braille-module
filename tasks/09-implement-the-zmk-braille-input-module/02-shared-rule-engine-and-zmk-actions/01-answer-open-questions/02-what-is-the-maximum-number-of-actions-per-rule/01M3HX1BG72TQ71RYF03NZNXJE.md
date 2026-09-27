---
created: Mon 28 Sep 2026 - 03h 01m 39s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: 'Decided: 16 actions per rule'
cargo:
  type: task
  status: delivered
---

# Decision: 16 actions per rule

## 1. Decision

1. A rule invokes at most 16 ordered ZMK behavior actions.
2. The one limit applies to factory rules, user overrides, dispatch, and persistence.
3. Overrides are persisted at variable length, so short rules stay small within the settings partition.

## 2. Rationale

1. A letter is one `&kp`, and a capital letter is one `&kp LS(X)`.
2. Optional Grade 2 words emit up to about 10 keystrokes, e.g. "themselves".

## 3. Impact

1. `braille_rules.h`, the dispatcher, and the persisted format use 16; the on-device editor may expose fewer.

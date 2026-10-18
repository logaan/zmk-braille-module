---
created: Sun 18 Oct 2026 - 19h 13m 13s
author:
  type: agent
  harness: pi
  model: gpt-5.6-terra
  name: task-reorg
subject: Scope split for context-free punctuation
cargo:
  type: task
  status: draft
---

# Scope split for context-free punctuation

## 1. Promoted signs

1.  Implement comma, semicolon, colon, exclamation mark, apostrophe, hyphen,
    and full stop now as unconditional UEB1 rules.
2.  Use the shared rule/action representation and preserve the appropriate
    class metadata for phase-04 context transitions.

## 2. Deferred sign

1.  Do not map dots 2, 3, 6 in this task.
2.  It is a question mark away from word start and an opening double quotation
    mark at word start, so phase 04 owns its context-conditioned rules.

## 3. Verification

1.  Test each promoted sign with the real phase-02 command table and alphabet.
2.  Preserve the dots-2,3,6 acceptance cases for the later contextual task.

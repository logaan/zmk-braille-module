---
created: Sun 18 Oct 2026 - 19h 13m 13s
author:
  type: agent
  harness: pi
  model: gpt-5.6-terra
  name: task-reorg
subject: Placement in the static-table phase
cargo:
  type: task
  status: draft
---

# Placement in the static-table phase

## 1. Rationale

1.  Each specified symbol has a fixed one-or-two-cell sequence and fixed host
    output.
2.  The delivered matcher already retains a strict prefix until the next chord
    and dispatches the matched rule, so no UEB context implementation is a
    prerequisite.

## 2. Requirements

1.  Implement the existing specified-symbol list unchanged in phase 03.
2.  Test each completed sequence and every first-cell non-continuation through
    production matching and dispatch.
3.  Validate the resulting UEB1 table against the real phase-02 command table.

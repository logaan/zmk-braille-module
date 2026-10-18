---
created: Sun 18 Oct 2026 - 19h 13m 13s
author:
  type: agent
  harness: pi
  model: gpt-5.6-terra
  name: task-reorg
subject: Scope retained for phase 04
cargo:
  type: task
  status: draft
---

# Scope retained for phase 04

## 1. Work moved earlier

1.  Direct factory space, navigation, and keyboard-action rules moved to phase
    02 because they need no context state.
2.  Context-free UEB1 signs and `computer8` rows moved to phase 03.

## 2. Remaining work

1.  This task now owns only context transitions, word boundaries, pending-input
    lifecycle completion, and their reset/event boundaries.
2.  Reuse the delivered generic matcher and the real earlier tables rather than
    introducing a second matching path.

## 3. Dependency

1.  Treat phase-02 space, Return, and Tab rules as the real whitespace inputs
    when implementing word-end behavior.

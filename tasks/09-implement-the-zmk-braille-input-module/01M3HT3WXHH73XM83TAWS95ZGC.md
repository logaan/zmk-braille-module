---
created: Mon 28 Sep 2026 - 02h 10m 36s
author:
  type: agent
  harness: pi
  model: unknown
subject: Second-pass plan review applied
cargo:
  type: task
  status: draft
---

# Second-pass plan review

1. Applied the interview outcomes; all implementation tasks remain draft for review.
2. Execution model: one agent, sequential, phase by phase; no parallel branches.
3. Each phase with open questions starts with an `01-answer-open-questions` decision milestone that must be resolved before that phase's implementation; phase 06 has none because its prerequisites were decided in phases 01 and 02.
4. Unit tests now live inside their implementation task. Removed four standalone unit-test tasks (chord timing, shared-engine fixtures, dot patterns/ASCII, persistence/precedence) and folded their coverage into the owning implementations.
5. Kept standalone tests only where they justify separate effort: two ICEB conformance suites, mode-switching integration, hardware-in-the-loop settings tests, and phase 06 acceptance.
6. Assigned `worker` on all 80 leaf tasks: 24 user (19 preference/policy/hardware decisions, 5 hardware acceptance), 56 agent (implementation, unit tests, research, docs, audits). Container folders carry no worker.
7. Fixed a false coupling I had introduced: factory chords come from the space-plus-dot pool while character tables use dot-only patterns, so phase 02 chord allocation and the phase 04 computer8 mapping do not block each other.
8. Added no handoff notes or extra status reporting; resume relies on the repository and task messages alone.
9. Agent worker model is recorded as `unknown` because the harness does not expose it; adjust if a specific model should be named.

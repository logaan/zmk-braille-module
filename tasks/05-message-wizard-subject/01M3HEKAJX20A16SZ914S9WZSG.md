---
created: Sun 27 Sep 2026 - 22h 49m 19s
author:
  type: agent
  harness: pi
  model: unknown
subject: Interactive wizard changes delivered
cargo:
  type: task
  status: delivered
---

# Results

1. Moved the subject prompt immediately before the body choice in interactive message creation.
2. Pre-fill the subject with `OldStatus -> NewStatus` when the selected status differs from the latest task status, while preserving supplied subjects.
3. Replaced the body confirmation with an explicit Yes/No choice before the editor; default yes for draft and cancelled task messages, questions, and answers, and no for other task statuses.
4. Added PTY coverage for prompt order, state changes, delivered -> done, body defaults and overrides.
5. `./scripts/test-td`, `./scripts/test-message-new-script-wizard`, and `./scripts/td message validate` pass.

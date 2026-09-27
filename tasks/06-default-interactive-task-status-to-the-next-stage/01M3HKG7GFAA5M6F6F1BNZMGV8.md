---
created: Mon 28 Sep 2026 - 00h 15m 00s
author:
  type: agent
  harness: pi
  model: unknown
subject: Status defaults follow the task progression
cargo:
  type: task
  status: delivered
---

# Results

1. Interactive task messages now suggest draft -> ready -> started -> delivered -> done based on the selected task's latest status.
2. Explicit status flags and imported message statuses take precedence; other statuses remain selectable.
3. Added PTY coverage for all progression stages, terminal statuses, overrides, and selection, plus a help-text check.
4. Verified with scripts/test-td and scripts/test-message-new-script-wizard.

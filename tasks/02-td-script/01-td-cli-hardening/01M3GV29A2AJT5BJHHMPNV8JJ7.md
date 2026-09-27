---
created: Sun 27 Sep 2006 - 17h 07m 00s
author:
  type: agent
  harness: pi
  model: gpt-6-sol
subject: Harden TaskDown CLI
cargo:
  type: task
  status: draft
---

# Harden TaskDown CLI

1. Review and schedule the four deferred findings from the `td` code review.
2. Child tasks:
    1. `01-confine-message-creation`: Keep new messages inside `tasks/`.
    2. `02-validate-created-datetime`: Validate created datetime ranges.
    3. `03-validate-ulid`: Reject invalid ULID first digits.
    4. `04-handle-body-file-errors`: Handle unreadable body files cleanly.
3. These tasks are drafts; do not implement them until they are scheduled.

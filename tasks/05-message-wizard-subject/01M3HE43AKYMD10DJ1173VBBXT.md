---
created: Sun 27 Sep 2026 - 22h 41m 00s
author: user
subject: Prompt for subject just before body in message wizard
cargo:
  type: task
  status: ready
---

# Prompt for subject just before body in message wizard

1. During interactive message creation, ask for the subject immediately before asking whether to enter a body.
2. When changing a task status, pre-fill the subject with `OldStatus -> NewStatus`.
3. Preserve provided subjects and cover prompt order and state changes in interactive tests.

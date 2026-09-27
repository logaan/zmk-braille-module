---
created: Sun 27 Sep 2026 - 23h 11m 03s
author: user
subject: Default interactive task status to the next stage
cargo:
  type: task
  status: ready
---

# Default interactive task status to the next stage

1.  When interactively creating a task message, default its status based on the task's existing status.
2.  Follow the normal progression: `draft -> ready -> started -> delivered -> done`.
3.  Keep all statuses selectable; the suggested status is only a default.

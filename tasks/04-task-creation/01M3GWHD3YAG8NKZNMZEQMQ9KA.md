---
author:
  harness: pi
  model: gpt-6-sol
  type: agent
cargo:
  status: draft
  type: task
created: Sun 27 Sep 2006 - 17h 33m 00s
subject: Add task creation to ./scripts/td
---

# Add task creation to `./scripts/td`

1.  Add a command to create tasks under `tasks/`.
    1.  Support top-level tasks and subtasks in an existing task.
    2.  Follow TaskDown folder numbering and naming conventions; avoid
        collisions.
2.  Create a valid initial task message in each new folder.
    1.  Accept a subject, status, and optional Markdown body.
    2.  Validate input before writing; do not leave an empty task on failure.
3.  Document the command in `--help` and cover success and failure cases in
    `scripts/test-td`.

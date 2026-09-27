---
author:
  harness: pi
  model: gpt-6-sol
  type: agent
cargo:
  status: draft
  type: task
created: Sun 27 Sep 2006 - 17h 03m 00s
subject: Keep new messages inside tasks/
---

# Keep new messages inside tasks/

1.  `td message new --task /tmp/outside --subject X --type task --status draft`
    writes outside `tasks/` when that directory exists.
2.  A symlink under `tasks/` pointing outside the root has the same effect.
3.  Reject absolute paths and ensure the resolved target directory remains
    inside the resolved `tasks/` root before writing.
4.  Add regression tests for absolute paths and symlinks.

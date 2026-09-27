---
created: Mon 28 Sep 2026 - 03h 25m 48s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: Skip, keep and notify invalid settings
cargo:
  type: task
  status: done
---

1. Each bad record is skipped individually but kept in storage, and a boot notice is shown; valid records still load.
2. Folded into "Persist stable behavior bindings".

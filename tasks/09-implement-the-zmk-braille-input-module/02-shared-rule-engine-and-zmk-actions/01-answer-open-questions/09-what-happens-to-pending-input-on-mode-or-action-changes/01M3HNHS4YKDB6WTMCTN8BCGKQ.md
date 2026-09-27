---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: What happens to pending input on mode or action changes?
cargo:
  type: task
  status: draft
  worker: user
---

# What happens to pending input on mode or action changes?

1. Resolve pending rule prefixes and UEB context when changing character tables, changing layers, entering/leaving settings, or invoking navigation/custom actions.
2. Apply spec §3.3's no-unintended-output requirement and coordinate with phase 01's physical chord lifecycle.
3. This is an initial-engine prerequisite, not a Grade 2-only decision; the extension must reuse it unless explicitly refined later.

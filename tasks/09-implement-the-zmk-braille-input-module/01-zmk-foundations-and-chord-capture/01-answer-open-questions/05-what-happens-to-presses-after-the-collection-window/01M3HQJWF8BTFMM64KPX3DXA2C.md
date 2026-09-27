---
created: Mon 28 Sep 2026 - 01h 26m 22s
author:
  type: agent
  harness: pi
  model: unknown
subject: What happens to presses after the collection window?
cargo:
  type: task
  status: draft
---

# What happens to presses after the collection window?

1. Decide how to handle a new press after the window closes but before the current chord's participating keys are all released.
2. Preserve spec §2.2: late presses cannot join that chord, and each collected chord resolves only once.
3. Record the press/release cases for the chord-detector tests; leave the policy open until this task is resolved.

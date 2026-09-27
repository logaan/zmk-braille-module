---
created: Mon 28 Sep 2026 - 00h 53m 45s
author: user
subject: What maximum output delay is acceptable in Grade 2?
cargo:
  type: task
  status: draft
  worker: user
---

# What maximum output delay is acceptable in Grade 2?

1. The engine has no timers: pending input resolves only on the next chord or is discarded by a transition.
2. Decide whether Grade 2 needs an output-delay bound beyond the pending cell count and, if so, how it is expressed without a timeout committing or dropping input.

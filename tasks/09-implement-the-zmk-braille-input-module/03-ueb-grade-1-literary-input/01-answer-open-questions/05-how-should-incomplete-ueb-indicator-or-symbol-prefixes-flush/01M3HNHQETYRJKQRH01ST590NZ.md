---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: How should incomplete UEB indicator or symbol prefixes flush?
cargo:
  type: task
  status: draft
  worker: user
---

# How should incomplete UEB indicator or symbol prefixes flush?

1. Identify Grade 1-specific incomplete and ambiguous prefixes that the shared pending-input policy must handle.
    1. That policy keeps input pending until the next chord extends it or rules it out, commits the longest complete match, and drops an orphan incomplete prefix with a display notice; there are no timers.
2. Record only necessary refinements to that policy; do not establish a separate Grade 1 dispatch path.
3. Distinguish rulebook-defined valid interpretation from firmware choices for abandoned or invalid input.

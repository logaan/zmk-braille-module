---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Match positional Grade 2 rules
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Match positional Grade 2 rules

1. Reuse the existing word-start, word-middle, and word-end constraints and longest-valid matching; add only the word-part eligibility context needed.
2. Resolve each buffered word as a whole, using its complete cell sequence as context for positional and word-part eligibility; output uncontracted text where no valid contraction applies.
3. Keep all resulting text and actions on the shared binding-dispatch path.
4. Unit-test positional matching alongside the implementation: word positions and word-part eligibility, longest-valid match with shorter and uncontracted fallback, and shared dispatch of the resulting actions.

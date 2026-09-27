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

1. Reuse existing word-start/middle/end constraints and longest-valid matching; add only necessary word-part eligibility context.
2. Use word boundaries, punctuation, and subsequent cells as evidence for commitment; output uncontracted text where no valid contraction applies.
3. Keep all resulting text/actions on the shared binding-dispatch path.
4. Unit-test positional matching alongside the implementation: word-start/middle/end and word-part eligibility, longest-valid match with shorter and uncontracted fallback, and shared binding dispatch for the resulting actions.

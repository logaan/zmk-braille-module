---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Isolate computer8 from UEB context
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Isolate computer8 from UEB context

1. Do not apply UEB numeric or capital indicators, word-position rules, or contraction buffering to the `computer8` table; its rules have no `when` predicate and use the `neutral` class of `references/research/ueb-indicator-context.yaml`.
2. Keep space, commands, and user overrides active; user-defined multi-chord rules still use the common sequence matcher.
3. A mode change discards pending input and resets UEB context; nothing carries over as `computer8` input.
4. Unit-test isolation alongside the implementation: UEB indicators, word-position rules, and contraction buffering do not apply, while space, commands, and user overrides remain active.

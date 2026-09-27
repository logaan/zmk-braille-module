---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Isolate computer8 from UEB context
cargo:
  type: task
  status: draft
---

# Isolate computer8 from UEB context

1. Do not apply UEB numeric/capital indicators, word-position rules, or contraction buffering to the computer8 character table.
2. Keep space/navigation and user overrides active; user-defined multi-chord rules still use the common sequence matcher where needed.
3. Clear or resolve prior UEB pending context according to the shared transition policy rather than reinterpreting it as computer8 input.

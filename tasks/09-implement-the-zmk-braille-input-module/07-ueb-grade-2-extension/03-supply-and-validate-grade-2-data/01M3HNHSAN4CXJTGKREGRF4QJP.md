---
created: Mon 28 Sep 2026 - 00h 50m 49s
author: user
subject: Supply and validate Grade 2 data
cargo:
  type: task
  status: draft
---

# Supply and validate Grade 2 data

1. Use ICEB third-edition rules and the same action representation as `ueb1`.
2. Take every contraction, its dot pattern, and its source from `references/research/ueb-grade2-contractions.yaml`; the table includes all 182, not a subset.
3. Implement the reading rules only; the rulebook's writing rules (pronunciation, preference, bridging, word division) choose whether a transcriber contracts and never change how a written sign reads.
4. Write per-rule unit tests alongside the data tasks; the rulebook-example and integration suites remain standalone.

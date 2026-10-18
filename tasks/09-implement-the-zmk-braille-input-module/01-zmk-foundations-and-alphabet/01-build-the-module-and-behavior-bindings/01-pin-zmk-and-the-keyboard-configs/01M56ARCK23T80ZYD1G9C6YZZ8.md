---
created: Sun 18 Oct 2026 - 12h 41m 55s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: leaf-01-baseline
subject: Module CI pin without an existing workflow
cargo:
  type: question
---

1. The module repository has no CI workflow; its existing west/west.yml now pins the verified release and tested community revisions, and both keyboards have their own CI configuration pinned.
2. Is retaining and documenting that authoritative development/CI manifest sufficient for this baseline leaf, with actual module CI build wiring left to the subsequent build/smoke leaves, rather than adding a new workflow that would overlap them?

---
created: Mon 28 Sep 2026 - 01h 26m 22s
author:
  type: agent
  harness: pi
  model: unknown
subject: How will rules represent UEB indicator context?
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# How will rules represent UEB indicator context?

1. Determine the minimal common rule and context representation for numeric mode, capitalization, and grade 1 indicators before fixing the schema.
2. Consult the pinned rulebook's indicator rules directly; phase 03's milestone may refine the representation, and any refinement is recorded there.
3. Preserve spec §4: one behavior-action representation, a thin driver, and no per-character or per-command dispatch branches.
4. Leave the mechanism open; a new translation engine or general-purpose rule language is not a requirement.

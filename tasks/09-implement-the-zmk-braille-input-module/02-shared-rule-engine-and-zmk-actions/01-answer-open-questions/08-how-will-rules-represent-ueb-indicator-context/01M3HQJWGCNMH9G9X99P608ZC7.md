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
---

# How will rules represent UEB indicator context?

1. Determine the minimal common rule/context representation for numeric mode, capitalization, and grade 1 indicators before fixing the schema.
2. Use the Grade 1 rulebook research in phase 03 as input; do not postpone these requirements until after the matcher is implemented.
3. Preserve spec §4: one behavior-action representation, a thin driver, and no per-character or per-command dispatch branches.
4. Leave the mechanism open; a new translation engine or general-purpose rule language is not a requirement.

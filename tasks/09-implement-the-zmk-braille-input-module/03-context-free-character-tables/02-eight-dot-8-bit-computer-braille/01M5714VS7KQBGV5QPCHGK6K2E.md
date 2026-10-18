---
created: Sun 18 Oct 2026 - 19h 13m 13s
author:
  type: agent
  harness: pi
  model: gpt-5.6-terra
  name: task-reorg
subject: Placement in the static-table phase
cargo:
  type: task
  status: draft
---

# Placement in the static-table phase

## 1. Rationale

1.  `computer8` uses fixed single-cell mappings and the already registered
    named-mode/table-selection path.
2.  Its rules are intentionally context-free and use the neutral class, so it
    does not depend on phase-04 UEB transitions.

## 2. Ordering

1.  Complete phase-02 factory command data first.
2.  Resolve and document command collisions, especially dot-7 and dot-8
    choices, against that real command table before publishing character rows.

## 3. Existing boundary

1.  A missing character rule already produces no host output and reaches the
    existing dropped-prefix notification hook.
2.  Phase 05 supplies visual presentation of that notification; it is not a
    prerequisite for exhaustive map and no-output tests here.

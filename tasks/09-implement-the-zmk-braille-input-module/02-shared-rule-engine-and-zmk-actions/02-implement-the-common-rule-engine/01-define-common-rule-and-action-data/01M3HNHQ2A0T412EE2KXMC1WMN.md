---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Define common rule and action data
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Define common rule and action data

1. Create `braille_rules.h` with bounded nine-bit chord sequences and `any`, `word-start`, `word-middle`, or `word-end` constraints.
2. Represent output uniformly as ordered ZMK behavior bindings: a device and up to two parameters per invocation.
3. Apply the resolved UEB-context design so numeric/capitalization/grade 1 state fits the shared matcher without per-character dispatch or separate output types.
4. Keep the shape usable by read-only factory tables, the initially empty override layer, and later persisted user sequences/actions.

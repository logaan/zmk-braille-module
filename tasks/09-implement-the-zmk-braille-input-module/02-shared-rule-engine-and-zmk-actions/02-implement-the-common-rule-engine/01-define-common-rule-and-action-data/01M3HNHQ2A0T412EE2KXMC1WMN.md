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

1. Create `braille_rules.h` with sequences of up to 8 nine-bit chords and `any`, `word-start`, `word-middle`, or `word-end` constraints.
2. Represent output uniformly as up to 16 ordered ZMK behavior bindings, each a behavior device and up to two parameters.
    1. There is no separate character, command, or custom-action output type.
3. Apply the resolved UEB-context representation so numeric, capitalization, and grade 1 state fit the shared matcher without per-character dispatch or separate output types.
4. Keep the shape usable by read-only factory tables, the initially empty override layer, and persisted user rules.

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
3. Represent UEB context as in `references/research/ueb-indicator-context.yaml`, without per-character dispatch or separate output types:
    1. Context fields `numeric`, `caps`, and `word_start`.
    2. On every rule, an optional `when` predicate over those fields and one class naming the kind of sign.
    3. On capitals indicator rules, the capitals mode they set.
    4. The `word-start` constraint is the `word_start` field.
4. Keep the shape usable by read-only factory tables, the initially empty override layer, and persisted user rules.

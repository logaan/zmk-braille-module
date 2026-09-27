---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Create factory space and navigation rules
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Create factory space and navigation rules

1. Add `braille_rules_nav.c`: Braille-space-only emits a literal space by default in every mode.
2. Add per-mode rules for left/right, line start/end, previous/next UI element, and the settings-entry action, using the researched chords with their source comments.
3. Unit-test the rules alongside the data: literal space and each command in each initial mode, against fixture character tables.

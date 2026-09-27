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

1. Add `braille_rules_nav.c`: space-only emits literal space; space-plus-dot rules cover left/right, line start/end, and previous/next UI element for VoiceOver-style use.

---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Test navigation and failure boundaries
cargo:
  type: task
  status: draft
  worker: user
---

# Test navigation and failure boundaries

1. Exercise left/right, line start/end, and previous/next UI-element factory navigation with the selected VoiceOver-style host setup in both initial modes.
2. Verify host-dependent actions and adaptations through overrides, mode, layer, and settings transitions, incomplete prefixes, and no duplicated/lost or unintended host output.
3. Reuse component tests for limits and persistence; add end-to-end checks that pending display, chosen latency bounds, and saved bindings remain consistent after reset/power cycle.

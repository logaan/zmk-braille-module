---
created: Mon 28 Sep 2026 - 00h 50m 49s
author: user
subject: Test optional ueb2 integration
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Test optional ueb2 integration

1. Confirm `ueb2` is selectable only when its rules are included, through both devicetree configuration and the on-device mode selector.
2. Verify its command table and overrides, contraction and fallback output, pending-cell display, buffering limits, and switching back to `ueb1` and `computer8`.
3. Re-run initial-mode regressions with Grade 2 included and omitted; the initial feature set must not depend on it.
4. Document the extension's implemented rule coverage, indicator and context behavior, and bounds alongside its tests; do not claim unsupported contractions.

---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Test the shared engine with fixture tables
cargo:
  type: task
  status: draft
---

# Test the shared engine with fixture tables

1. Use small character/navigation tables and in-memory override fixtures; do not depend on phases 03–05 being implemented.
2. Cover longest-valid matching, shorter fallback with remaining input, all word positions, ambiguous/unmatched prefixes, and buffer/latency limits.
3. Verify override precedence for identical inputs without letting a shorter override incorrectly defeat a longer valid sequence; distinguish real factory collisions from valid prefixes/context variants.
4. Verify action ordering, zero/one/two parameters, modifier lifecycle, and uniform space/dot mask handling.
5. Test named-table selection and transition/pending-state contracts; exercise actual tables and saved overrides in their later phases.

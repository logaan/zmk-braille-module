---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: What happens to a pending chord when Braille input is disabled?
cargo:
  type: task
  status: draft
  worker: user
---

# What happens to a pending chord when Braille input is disabled?

1. Define treatment of the collection window and held/released role keys when disabling Braille input or entering/leaving settings capture.
2. Spec §3.3 already forbids unintended output; resolve the lifecycle details without weakening that requirement.
3. Coordinate with phase 02's pending-engine-input policy; raw chord collection and an unresolved multi-chord rule are different states.

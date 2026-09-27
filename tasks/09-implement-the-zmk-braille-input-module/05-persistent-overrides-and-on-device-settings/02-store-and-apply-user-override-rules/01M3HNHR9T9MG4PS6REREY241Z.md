---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Store and apply user override rules
cargo:
  type: task
  status: draft
---

# Store and apply user override rules

1. Use a separate, empty-by-default override layer; factory tables remain read-only.
2. Write each storage unit test alongside its implementation task; real-display and post-reboot verification are separate tasks.

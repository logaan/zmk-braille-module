---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Confirm, save, remove and reset
cargo:
  type: task
  status: draft
---

# Confirm, save, remove and reset

1. Show the proposed input sequence and action/parameters for confirmation before saving, unlike immediate character-mode selection.
2. Return to the normal display after a successful save; cancellation leaves the saved binding unchanged, and failed/capacity-limited saves remain visible as failures.
3. Expose removal of one override and reset of all overrides to factory defaults without resetting unrelated keyboard settings.

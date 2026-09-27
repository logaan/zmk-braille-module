---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Test saved action replay after reboot
cargo:
  type: task
  status: draft
---

# Test saved action replay after reboot

1. Create/edit supported bindings through the UI, then verify saved parameters and invocation after reset and power cycle without a host configuration tool or reflash.
2. Verify deletion and factory restoration persist; use the storage tests' fixtures for multi-action shapes the on-device editor does not expose.
3. Do not require a general multi-action editor merely to repeat the common dispatch/persistence tests.

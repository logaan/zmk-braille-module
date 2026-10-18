---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: ZMK foundations and alphabet
cargo:
  type: task
  status: draft
---

# ZMK foundations and alphabet

1. Pin the baseline, build the out-of-tree module, and implement the Braille keys, release-terminated chord capture, the braille-mode-key, and the settings-entry action.
2. Build the common rule and action data, matcher, ordered action dispatch, and named-table registration; implement the 26 six-dot alphabet cells immediately after those prerequisites.
3. End this phase with real lowercase alphabet rules on the shared capture, matching, and host-action path, verified by the user-owned hardware test immediately after the alphabet task.
4. Lowercase alphabet rules can run in the initial UEB context; phase 02 adds context transitions, word boundaries, the pending-input lifecycle, and real factory space and command rules.
5. Use fixtures for later tables and overrides while building the common path; settings screens remain in phase 05.

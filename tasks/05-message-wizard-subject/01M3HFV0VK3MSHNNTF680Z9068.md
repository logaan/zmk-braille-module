---
created: Sun 27 Sep 2026 - 23h 11m 00s
author:
  type: agent
  harness: pi
  model: unknown
subject: No-body editor bypass delivered
cargo:
  type: task
  status: delivered
---

# Results

1. Choosing No for a body now skips `$EDITOR` and proceeds directly to the message preview and creation confirmation.
2. Choosing Yes still opens the editor for the full message.
3. Updated `scripts/test-message-new-script-wizard` to assert that no-body choices never invoke the editor, including delivered -> done and pre-filled bodies that are declined.
4. `./scripts/test-td`, `./scripts/test-message-new-script-wizard`, and `./scripts/td message validate` pass.

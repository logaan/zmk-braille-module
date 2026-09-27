---
created: Sun 27 Sep 2006 - 18h 52m 00s
author:
  type: agent
  harness: pi
  model: unknown
subject: Multiline body entry delivered
cargo:
  type: task
  status: delivered
---

# Results

1. `scripts/examples/message-new-gum.bb` uses JLine for body entry because Gum write hard-codes Enter as submit; Enter inserts a newline and Ctrl-Y finishes.
2. Added PTY coverage in `scripts/test-message-new-gum` for multiline, question validation, empty bodies, declined confirmation, Ctrl-C, and Ctrl-D.
3. Verified with `./scripts/test-message-new-gum`, `./scripts/test-td`, `./scripts/test-message-new-script-wizard`, and clj-kondo.

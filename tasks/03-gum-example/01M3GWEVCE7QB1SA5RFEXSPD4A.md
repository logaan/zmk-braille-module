---
created: Sun 27 Sep 17h32
author:
  type: agent
  harness: pi
  model: gpt-6-sol
subject: Gum choices now support typed filtering
cargo:
  type: task
  status: delivered
---

# Results

1. Changed Type and Status prompts in `scripts/examples/message-new-gum.bb` from `gum choose` to Gum’s built-in `gum filter --fuzzy --strict`.
2. Users can type to narrow choices or use arrow keys; only listed options can be returned.
3. Verified typed type/status choices and arrow navigation using live Gum in a pseudo-terminal; `./scripts/test-td` and `./scripts/td message validate` passed.

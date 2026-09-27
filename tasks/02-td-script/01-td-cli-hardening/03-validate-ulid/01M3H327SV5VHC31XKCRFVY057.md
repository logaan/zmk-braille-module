---
created: Sun 27 Sep 2026 - 19h 27m 45s
author:
  type: agent
  harness: pi
  model: openai/gpt-5
subject: Results
cargo:
  type: task
  status: delivered
---

# Results

1. Reject message filenames whose ULID first digit is outside 0-7.
2. Apply the same bound to truncated ULIDs in answer references.
3. Added boundary tests for first digits 0, 7, 8 and Z.
4. `./scripts/test-td` and `./scripts/td message validate` pass.

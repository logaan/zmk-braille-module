---
created: Mon 28 Sep 2026 - 00h 21m 06s
author:
  type: agent
  harness: pi
  model: unknown
subject: Uniform prompt labels delivered
cargo:
  type: task
  status: delivered
---

# Results

1. Applied a consistent muted background to interactive prompt labels; `NO_COLOR` and `TERM=dumb` retain plain labels.
2. All wizard prompts end in `:`; the subject prompt is `Subject:` while the 64-character validation remains.
3. Added PTY checks for prompt styling, resets before input, and the no-color path.
4. `./scripts/test-td`, `./scripts/test-message-new-script-wizard`, and `./scripts/td message validate` pass.

---
created: Mon 28 Sep 2026 - 02h 16m 33s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: 'Note: agent model metadata'
cargo:
  type: task
  status: started
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: coordinator
---

# Note: agent model metadata

1.  All subagents on this task run on `qwen/qwen3.8-max-0902` (your pi status
    bar shows it).
2.  When posting messages with `td`, always pass
    `--model qwen/qwen3.8-max-0902` (author) and
    `--worker-model qwen/qwen3.8-max-0902` (worker).
3.  Do not record or use `gpt-6-sol` / any gpt model: that is only the unused
    settings default.
4.  vault-01's earlier messages were corrected by the coordinator; nav-01,
    view-01 and ctrl-01 should use the right value from their first message.

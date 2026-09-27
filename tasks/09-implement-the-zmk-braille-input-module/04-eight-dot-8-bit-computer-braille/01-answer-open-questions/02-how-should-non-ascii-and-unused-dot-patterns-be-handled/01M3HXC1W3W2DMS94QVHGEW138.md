---
created: Mon 28 Sep 2026 - 03h 07m 29s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: 'Decided: reserved patterns give no output and a notice'
cargo:
  type: task
  status: delivered
---

# Decision: no output and a display notice

## 1. Decision

1. A reserved pattern (no printable ASCII character) sends nothing to the host.
2. The display briefly shows the pattern as unassigned.
3. User overrides can bind reserved patterns to any action.

## 2. Impact

1. The computer8 table marks reserved entries; the pending-cell display shows the notice.

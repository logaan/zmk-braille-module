---
created: Sun 01 Nov 2026 - 00h 34m 57s
author:
  type: agent
  harness: codex
  model: gpt-6.1-sol
  name: shorthand-design
subject: Implement automatic multi-cell shorthand over Computer8
cargo:
  type: task
  status: started
---

1. Implement the clarified fourth selectable mode with Computer8 fallback and automatic multi-cell contractions.
2. Exclude the 62 single-cell contraction conflicts and retain all Computer8 character and command meanings.
3. Reuse bounded word buffering, override precedence and read-only preview; do not interpret UEB indicators as silent Computer8 input.
4. Cover every single-cell Computer8 input and the 120 multi-cell mappings, positions, commands, reset boundaries and overrides.

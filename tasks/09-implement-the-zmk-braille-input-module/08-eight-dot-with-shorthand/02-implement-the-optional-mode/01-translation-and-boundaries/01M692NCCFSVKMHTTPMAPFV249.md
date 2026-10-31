---
created: Sun 01 Nov 2026 - 00h 33m 53s
author:
  type: agent
  harness: codex
  model: gpt-6.1-sol
  name: shorthand-audit
subject: Constraints for an additive shorthand namespace
cargo:
  type: task
  status: draft
---

1. If the user chooses a reserved-cell prefix, treat its scope as transient input state, not a persistent switch to UEB2.
2. Specify completion, whitespace, navigation, cancellation, layer/mode/settings resets, unknown input, and the pending bound before implementation.
3. Keep existing user overrides ahead of the new prefix and shorthand rules, including overrides of the prefix itself.
4. The display must distinguish Computer8 literals from explicitly armed shorthand and must not dispatch or mutate input while previewing.
5. These are review constraints, not approval of a particular new spelling.

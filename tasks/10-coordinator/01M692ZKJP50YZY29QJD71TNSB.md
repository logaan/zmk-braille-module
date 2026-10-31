---
created: Sun 01 Nov 2026 - 00h 39m 28s
author:
  type: agent
  harness: codex
  model: gpt-6.1-sol
  name: publication-checks
subject: Published firmware CI and complete baseline tests pass
cargo:
  type: answer
---

1. Published Toucan d937941 passed workflow 41205917402, including host tests, keymap drawing and all five firmware targets. Firmware and keymap build-50 artifacts are available.
2. Published QBF d0d013c passed workflow 41205918241, including left, right and settings reset. Firmware build-25 artifact is available. Both published manifests resolve module c8dfed8.
3. All 82 host checks in committed snapshot 764ed49 passed, including the repaired 11 coordinator tests. Full discovery passed 80 and skipped two optional PDF checks; both then passed when the existing local rulebook was linked into the archived snapshot.
4. Testing used the committed snapshot to avoid the new shorthand feature being edited concurrently. No worker, model or hardware was launched by the coordinator tests.
5. README.md and docs/verification.md now document the repaired tests and successful published workflows. The module has no separate GitHub Actions workflow.
6. Reconciled the one stale optional UEB2 software integration leaf in f605288. The remaining pre-existing unfinished leaves are explicitly user-owned hardware acceptance tasks, and remain unclaimed.

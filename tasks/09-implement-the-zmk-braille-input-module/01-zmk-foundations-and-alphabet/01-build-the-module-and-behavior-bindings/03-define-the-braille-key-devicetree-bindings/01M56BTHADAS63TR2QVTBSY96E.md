---
created: Sun 18 Oct 2026 - 13h 00m 34s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: coordinator
subject: 'Coordinator assignment: leaf-03'
cargo:
  type: task
  status: started
  worker:
    type: agent
    harness: pi
    model: gpt-6.1-sol
    name: leaf-03-define-the-braille-key-devicetree-bindings
---

1. Assign this leaf to `leaf-03-define-the-braille-key-devicetree-bindings`; implement only this leaf and stop after its delivery.
2. The user delegates readiness transitions to the coordinator. This leaf has been marked ready after prerequisite review; do not change other leaves or mark your own work done.
3. Coordinate through this TaskDown task: read all messages before starting, and check for new coordinator feedback before finalizing. Put questions, blockers, decisions, and results here. Never send text into the coordinator pane.
4. Preserve unrelated edits. Commit only your own changes. Keep durable communication in TaskDown, not separate handoff files.
5. Scaffolding reviewed and coordinator-rerun host checks pass: python3 tests/host/test_build.py and PYTHONPATH=build/baseline/zephyr/scripts/kconfig python3 tests/host/test_kconfig.py. All TaskDown messages validate. Production behavior compatibles are already reserved in src/behavior_braille.c; read the prior delivery for registration/link evidence.
6. Provide production schemas/default nodes and per-keyboard named default mode, with repeatable parameter/schema boundary tests and a real build using them. Keep runtime routing/capture/mode/settings implementations for their later leaves.
7. Reuse build/baseline workspace and the existing Docker image; exact build mounts/commands are in prerequisite deliveries. Keyboard keymap inclusion, persistent module manifests, and the complete both-keyboard smoke/CI matrix belong to the next leaf, so avoid making sibling changes prematurely.

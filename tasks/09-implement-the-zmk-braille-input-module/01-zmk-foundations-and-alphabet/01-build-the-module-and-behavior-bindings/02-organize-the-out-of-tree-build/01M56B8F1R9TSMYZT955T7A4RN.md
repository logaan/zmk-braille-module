---
created: Sun 18 Oct 2026 - 12h 50m 42s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: coordinator
subject: 'Coordinator assignment: leaf-02'
cargo:
  type: task
  status: started
  worker:
    type: agent
    harness: pi
    model: gpt-6.1-sol
    name: leaf-02-organize-the-out-of-tree-build
---

1. Assign this leaf to `leaf-02-organize-the-out-of-tree-build`; implement only this leaf and stop after its delivery.
2. The user directly authorizes sequential phase-01 implementation through the alphabet checkpoint despite the earlier draft gate; do not mark your own work ready.
3. Coordinate through this TaskDown task: read all messages before starting, and check for new coordinator feedback before finalizing. Put questions, blockers, decisions, and results here. Never send text into the coordinator pane.
4. Preserve unrelated edits. Commit only your own changes. Keep durable communication in TaskDown, not separate handoff files.
5. Baseline delivery reviewed: all six UF2 artifacts exist, actual generated configs retain displays/split/pointing/Studio, ZMK/community checkouts are clean, and both remote baseline workflows passed. Read its delivery for exact commands and pins.
6. Reuse the pinned build/baseline workspace and zmkfirmware/zmk-build-arm:3.5 image; do not redo release research or download a second workspace unnecessarily. No west executable is installed on the host; use Docker.
7. Keep this to build/layout scaffolding and minimal behavior registration. Devicetree bindings/default nodes are the next leaf; event routing, capture, matching, and tables are later leaves. Avoid pretending placeholders implement them.
8. The repository currently has no module CI workflow. Authoritative pinning is complete; repeatable full module smoke-build/CI wiring may be added in leaf 04, after bindings exist. Preserve unrelated in-flight edits to AGENTS.md and scripts/task-down/task-down.

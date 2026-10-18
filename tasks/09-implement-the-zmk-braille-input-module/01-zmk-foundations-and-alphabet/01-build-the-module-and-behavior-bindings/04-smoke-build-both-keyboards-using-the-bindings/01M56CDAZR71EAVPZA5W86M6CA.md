---
created: Sun 18 Oct 2026 - 13h 10m 50s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: coordinator
subject: 'Coordinator assignment: leaf-04'
cargo:
  type: task
  status: started
  worker:
    type: agent
    harness: pi
    model: gpt-6.1-sol
    name: leaf-04-smoke-build-both-keyboards-using-the-bindings
---

1. Assign this leaf to `leaf-04-smoke-build-both-keyboards-using-the-bindings`; implement only this leaf and stop after its delivery.
2. The user delegates readiness transitions to the coordinator. This leaf has been marked ready after prerequisite review; do not change other leaves or mark your own work done.
3. Coordinate through this TaskDown task: read all messages before starting, and check for new coordinator feedback before finalizing. Put questions, blockers, decisions, and results here. Never send text into the coordinator pane.
4. Preserve unrelated edits. Commit only your own changes. Keep durable communication in TaskDown, not separate handoff files.
5. Binding delivery reviewed; coordinator reran all 6 devicetree tests, 2 build-contract tests, and 2 Kconfig tests successfully. Use #include <behaviors/braille.dtsi>, labels bc/bspc/bmode/bsettings, and the braille node default-mode property. All callbacks are still explicitly unsupported until later leaves.
6. Implement actual sibling manifest/config/keymap inclusion and Perkins layouts as required. Keep conventional Toucan typing and an obvious Braille-layer return path. Preserve baseline displays, Studio, pointing, and existing useful layers; no ZMK/core or community patches.
7. Reuse build/baseline and Docker 3.5. Provide a repeatable module-owned build/smoke entry point if useful, and verify default ueb1 plus a configured named mode at the build boundary. Do not require populated tables or claim runtime input works yet.
8. Read sibling contribution notes. The baseline delivery records user permission to push to their repositories. If pushing dependent keyboard configs, publish the exact module revision they reference first so remote builds can resolve its bindings. Existing sibling CI paths plus documented pinned development builds are sufficient; avoid unrelated CI frameworks.

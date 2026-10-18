---
created: Sun 18 Oct 2026 - 12h 35m 53s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: coordinator
subject: Coordinate through TaskDown; baseline compatibility
cargo:
  type: task
  status: started
---

1. The user requests coordination through TaskDown as much as possible.
    1. Read new messages in this task before finalizing work or declaring a blocker.
    2. Put questions, blockers, and delivery results here; do not send text into the coordinator pane.
    3. The coordinator will use tmux only to observe or notify you of new TaskDown messages.
2. Keep task tracking exclusively in ./tasks through ./scripts/td; unrelated legacy stores are outside this assignment.
3. For the pinned baseline, minimally adapting keyboard-owned code or configuration to the release is within this leaf when necessary.
    1. Do not patch ZMK core or fork/patch community modules.
    2. Do not silently remove required displays or other baseline hardware to obtain a pass.
    3. If the normal baseline remains incompatible, record the precise blocker and the decision needed here before proceeding to later tasks.

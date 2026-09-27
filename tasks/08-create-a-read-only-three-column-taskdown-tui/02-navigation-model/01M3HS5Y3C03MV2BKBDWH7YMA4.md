---
created: Mon 28 Sep 2026 - 01h 54m 15s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: Build the navigation model
cargo:
  type: task
  status: ready
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: nav-01
---

# Build the navigation model

1.  Read every message in the parent task folder
    `tasks/08-create-a-read-only-three-column-taskdown-tui/` first. The
    Architecture message is the binding contract: state and row shapes
    (3.4-3.5), your function contract (4.2), rules 5-7.
2.  You own ONLY these paths (never touch anything else):
    1.  `scripts/task-down/src/task_down/model.clj`
    2.  `scripts/task-down/test/task_down/model_test.clj`
3.  vault-01 is building the loader in parallel. Do NOT require
    `task-down.vault` and do NOT read fixtures from disk; build task-node and
    message literals inline in your tests per the data shapes.
4.  Deliverables (all pure), exactly the semantics of Architecture 4.2:
    1.  `initial-state`, `ancestor`, `rows`, `cursor-row-index`, `move-cursor`,
        `move-focus`.
    2.  Parents pane rows: only tasks that have child tasks, recursively pruned
        (a kept task shows only its kept children).
    3.  Listing pane rows: the full subtree under its ancestor.
    4.  Messages pane rows: heading then that task messages oldest first;
        `:row/expanded?` true only for the most recent message.
    5.  Heading row is row 0 of every pane.
    6.  Navigation: up/down = previous/next sibling (messages: previous/next
        message); left = parent (depth-0 task -> heading; heading -> noop);
        right = first child (heading -> first row; leaf -> noop); top/bottom =
        first/last row; missing target = unchanged state.
    7.  Ancestor-change resets: moving the parents cursor resets the listing
        cursor to `:heading`; moving the listing cursor resets the messages
        cursor to the most recent message id, or `:heading` when the task has
        no messages.
    8.  Focus: `:left`/`:right` wrap around the three panes
        (`:parents` <-left- `:listing` <-left- `:messages` <-left- `:parents`).
5.  Tests: small literal trees covering every rule above; be thorough on
    `move-cursor` edges (noops, wrapping boundaries, heading transitions) and
    on `rows` for all three panes, including the pruned parents pane.
6.  Verification: `cd scripts/task-down && bb test` passes (once bb.edn exists;
    until then use the one-liner in Architecture 6.2).
7.  Process (Architecture 7): post `ready -> started` now; commit only your
    owned paths; post a results message with status `delivered` when done. Do
    not set `done`. Questions go in this folder as `question` messages.

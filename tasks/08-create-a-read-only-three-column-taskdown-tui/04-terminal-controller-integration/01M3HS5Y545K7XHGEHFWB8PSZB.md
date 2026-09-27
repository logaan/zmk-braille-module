---
created: Mon 28 Sep 2026 - 01h 54m 15s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: Build terminal layer, controller and integration
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: ctrl-01
---

# Build the terminal layer, controller and integration

1.  Read every message in the parent task folder
    `tasks/08-create-a-read-only-three-column-taskdown-tui/` first. The
    Architecture message is the binding contract: pod choice (1), state shape
    (3.4), your contracts (4.4-4.6), rules 5-7.
2.  You own ONLY these paths (never touch anything else):
    1.  `scripts/task-down/src/task_down/terminal.clj`
    2.  `scripts/task-down/src/task_down/controller.clj`
    3.  `scripts/task-down/src/task_down/main.clj`
    4.  `scripts/task-down/test/task_down/controller_test.clj`
    5.  `scripts/task-down/task-down` (launcher)
    6.  `scripts/task-down/bb.edn` (extend only if required; note it in your
        messages)
3.  Prerequisite: vault-01, nav-01 and view-01 have delivered and `bb test` is
    green. The coordinator flips this task to `ready` when that is true; do not
    start before then.
4.  Deliverables:
    1.  `Terminal` protocol + `lanterna-terminal` record (Architecture 4.4);
        load the pod per Architecture 1.4; normalise keys to `{:key \j}`,
        `{:key :enter}`, `{:key :escape}`; ignore anything else.
    2.  `start!`: load the vault (dir from first CLI arg, default `tasks`),
        build initial state with `model/initial-state`, enter the screen, run
        the loop, restore the terminal on exit INCLUDING on exceptions
        (try/finally around the loop; stop! always runs).
    3.  Event loop (Architecture 4.5): render via `view/render`, blocking key
        read, apply `move-cursor`/`move-focus` per key map (`q` quit, `j k h l`,
        `g g` top via `:state/pending-key`, `G` bottom, enter focus-left,
        escape focus-right), re-read `size!` each iteration and update
        `:state/size`.
    4.  `task-down.main/-main` entry point; `task-down` launcher script
        (`#!/usr/bin/env bb`) runnable from the repo root:
        `scripts/task-down/task-down [vault-dir]`.
    5.  Controller tests with a fake `Terminal` record: scripted keys drive the
        loop; assert resulting states, that `draw!` receives ops, that `q`
        exits and `stop!` runs even when the loop throws.
    6.  Manual verification in your tmux pane against the real vault
        (`./scripts/task-down/task-down` from the repo root): three panes
        render with headings, colours by depth, navigation, focus switch,
        gg/G, collapsed messages, clean quit (terminal restored).
    7.  If Lanterna misbehaves on this machine (pod/keys/colours), implement
        the ANSI fallback Terminal behind the same protocol (Architecture 1.3)
        and explain in your delivered message.
5.  Verification: `cd scripts/task-down && bb test` green for the whole
    project, plus the manual run above.
6.  Process (Architecture 7): post `ready -> started` when you begin; commit
    only your owned paths; post a results message with status `delivered` when
    done, including anything you had to decide or deviate on. Do not set
    `done`. Questions go in this folder as `question` messages.

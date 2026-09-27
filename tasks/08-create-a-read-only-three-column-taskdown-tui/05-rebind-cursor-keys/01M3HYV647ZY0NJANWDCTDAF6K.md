---
created: Mon 28 Sep 2026 - 03h 33m 14s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: 'Rebind cursor keys: visual j/k, sibling braces'
cargo:
  type: task
  status: ready
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: keys-01
---

# Rebind cursor keys: visual j/k, sibling braces

1.  Read every message in the parent task folder
    `tasks/08-create-a-read-only-three-column-taskdown-tui/` first. The
    Architecture message plus its Amendment (cursor key bindings,
    `01M3HYP4P8J4DKAZKAVQ2GAF9W`) are the binding contract; the amendment
    supersedes the old bindings.
2.  You own ONLY these paths (the original authors are finished; nothing else
    may change):
    1.  `scripts/task-down/src/task_down/model.clj`
    2.  `scripts/task-down/test/task_down/model_test.clj`
    3.  `scripts/task-down/src/task_down/controller.clj`
    4.  `scripts/task-down/test/task_down/controller_test.clj`
3.  Deliverables:
    1.  `model/move-cursor` gains `:next` and `:prev` per the amendment: flat
        visual row order within the pane (heading row included), noop at the
        first/last row, and the same ancestor-reset and same-row-noop rules as
        the existing directions. `:up`/`:down` keep sibling semantics.
        Remember rows can skip pruned siblings in the parents pane; visual
        movement follows `rows`, not the raw tree.
    2.  Controller key map per the amendment: `{` -> model `:up`, `}` -> model
        `:down`, `j` -> model `:next`, `k` -> model `:prev`. `h`/`l`, `gg`/`G`,
        `enter`/`escape`, `q` unchanged.
    3.  Verify in a real tty that the pod delivers `{` and `}` as the
        Characters `\{` and `\}` (they are shifted keys). If it does not, post
        a question message here BEFORE inventing a workaround.
    4.  Tests: model coverage for `:next`/`:prev` (task panes including
        heading transitions and pruned-tree gaps, messages pane, edge noops,
        ancestor resets) and controller coverage for the full new key map
        including `{`/`}` through the fake Terminal.
    5.  Update every docstring that mentions the old bindings (`move-cursor`
        in model.clj, `key-actions` in controller.clj) so docs match behaviour.
4.  Constraints: the existing public API and data shapes stay exactly as they
    are; this is an additive change. Keep files near the ~200-line guideline
    (the two test files are already accepted at 229/236 - grow them only as
    much as the new coverage requires). No macros, no multimethods, one atom.
5.  Verification before delivering:
    1.  `cd scripts/task-down && bb test` green (57+ tests).
    2.  `clj-kondo --lint src test` clean.
    3.  Manual tty run (a dedicated tmux window, killed afterwards): `j`/`k`
        move visually across whatever row is below/above (sibling, parent or
        child); `{`/`}` step siblings; `q` restores the terminal.
6.  Process: post `ready -> started`; commit only your owned paths using
    explicit pathspecs (`git commit -- <paths>`) because other agents share
    this index; post a delivered results message; record your model as
    `qwen/qwen3.8-max-0902` in every `td` message. Your final actions: kill any
    probe windows you created, then close your own pane with
    `tmux kill-pane -t $TMUX_PANE`.

---
created: Mon 28 Sep 2026 - 02h 21m 46s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: nav-01
subject: 'Results: navigation model'
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: nav-01
---

# Results

1.  Delivered the pure navigation model in `scripts/task-down/`.

2.  Files created, both owned by nav-01, nothing else touched:
    1.  `src/task_down/model.clj` - 220 lines, namespace `task-down.model`.
    2.  `test/task_down/model_test.clj` - 12 tests, 117 assertions, all literal
        fixtures, no filesystem access.

3.  Public API, exactly Architecture 4.2:
    1.  `(initial-state vault size) -> state`
    2.  `(ancestor state pane) -> node`
    3.  `(rows state pane) -> vector of row maps`, heading always at index 0.
    4.  `(cursor-row-index state pane) -> int`, 0 when the cursor is not
        displayed.
    5.  `(move-cursor state pane direction) -> state`
    6.  `(move-focus state direction) -> state`

4.  Semantics implemented:
    1.  `:parents` rows keep only tasks that have child tasks, pruned at every
        generation; a kept task shows only its kept children.
    2.  `:row/path` is always a vault-root index path, so pruning never
        renumbers siblings (`03 Serve` stays `[2]` behind a pruned leaf).
    3.  `:listing` rows are the whole subtree under the ancestor, and
        `:row/depth` restarts at 0 for the ancestor's own children.
    4.  `:messages` rows are the heading then that task's messages oldest
        first, `:row/expanded?` true for the newest message only.
    5.  `up`/`down` step between siblings in the task panes, and between every
        row of the messages pane, so the heading is reachable from the first
        message (Architecture 4.2.5.1).
    6.  `left` is the parent (depth 0 -> heading, heading -> noop), `right` is
        the first displayed child (heading -> first row, leaf -> noop).
    7.  `top`/`bottom` are the first and last row of the pane.
    8.  A direction with no target returns the identical state, and so does one
        that lands on the cursor's own row; otherwise `gg` on a heading would
        needlessly reset the panes below it.
    9.  Moving the parents cursor resets the listing cursor to `:heading` and
        the messages cursor to the newest message of the new ancestor, or
        `:heading`, because the messages ancestor moved with it.
    10. Moving the listing cursor resets the messages cursor the same way;
        moving the messages cursor leaves the other cursors alone.
    11. `move-focus` wraps `:parents` `:listing` `:messages` both ways.

5.  Decisions where the contract left room, all recorded in docstrings:
    1.  `initial-state` focuses `:parents`, the leftmost pane.
    2.  In the messages pane `left`/`right` treat the heading as the messages'
        parent (message -> heading, heading -> first message), the same
        one-level tree the rows already describe.
    3.  In the task panes `up` from a first sibling is a noop; the heading is
        reached with `left`, as Architecture 4.2.5.2 requires.

6.  Compatibility notes for view-01 and ctrl-01:
    1.  Rows carry exactly the Architecture 3.5 keys: heading rows
        `:row/kind`/`:row/node`, task rows add `:row/path`/`:row/depth`,
        message rows add `:row/message`/`:row/expanded?`.
    2.  The model reads only `:task/children`, `:task/messages` and `:msg/id`,
        so vault-01's OrderedMap caveat does not reach it.
    3.  `:state/pending-key` is initialised to nil and left alone; the
        controller owns the `gg` sequence.
    4.  Every function is pure: no requires, no I/O, no atoms.

7.  Verification:
    1.  `cd scripts/task-down && bb test` -> `Ran 50 tests containing 220
        assertions. 0 failures, 0 errors.` exit 0 (model, vault and view tests;
        view-test exercises `task-down.model` through `render`).
    2.  `model-test` alone: `Ran 12 tests containing 117 assertions.
        0 failures, 0 errors.` exit 0.
    3.  `clj-kondo --lint` on both files: 0 errors, 0 warnings.
    4.  Smoke test on the real vault via `vault/load-vault`: 30 parents rows,
        ancestors and cursor indices as expected after `right`, `right`, `up`.

8.  Engineering rules held: the longest function body is 10 lines; no macros
    defined, no multimethods; recursion only in `task-rows` and `ancestor`;
    `model.clj` is 220 lines, roughly 50 of them docstrings and blank lines,
    which is as close to the ~200 guideline as I could get without stripping
    docstrings from private helpers.

9.  No open questions.

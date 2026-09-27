---
created: Mon 28 Sep 2026 - 01h 54m 15s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: Build the pane view
cargo:
  type: task
  status: ready
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: view-01
---

# Build the pane view

1.  Read every message in the parent task folder
    `tasks/08-create-a-read-only-three-column-taskdown-tui/` first. The
    Architecture message is the binding contract: shapes (3.3-3.6), your
    function contract (4.3), rules 5-7.
2.  You own ONLY these paths (never touch anything else):
    1.  `scripts/task-down/src/task_down/view.clj`
    2.  `scripts/task-down/test/task_down/view_test.clj`
3.  nav-01 is building `task-down.model` in parallel. You MAY require it
    (pure -> pure: `rows`, `cursor-row-index`, `ancestor`); you must NOT
    require vault, terminal or controller. To run your tests before nav-01
    delivers, write a minimal stub implementing Architecture 4.2 at
    `/tmp/view-stub-src/task_down/model.clj` and run with src FIRST on the
    classpath so the real model shadows the stub once it exists:
    `bb --classpath src:/tmp/view-stub-src:test -e "(require '[clojure.test :as t] 'task-down.view-test)(t/run-tests 'task-down.view-test)"`
    Never commit the stub; delete it once the real model.clj is in place.
4.  Deliverables (all pure), exactly Architecture 4.3:
    1.  `render` -> vector of draw-op maps.
    2.  Layout: pane width 80; pane x offsets 1, 82, 163; heading (uppercase,
        centred) on row 0; content from row 1.
    3.  Word wrapping at the pane width (indentation counts); over-long words
        split at the width.
    4.  Task name colour by generation depth, cycling
        `[:red :green :yellow :blue :magenta :cyan]` (depth 0 = red).
    5.  Cursor row: styles `#{:reverse}` when its pane is focused,
        `#{:underline}` when not.
    6.  Derived vertical scroll (not stored): keep the cursor row visible,
        otherwise render from the top.
    7.  Collapsed message row: created datetime with the year token removed,
        two spaces, subject. Example:
        `Mon 28 Sep - 00h 07m 57s  Create a read-only three-pane TaskDown TUI`.
    8.  Expanded message rows: subject line, author line, created line, blank
        line, then the raw markdown body.
    9.  Empty panes (e.g. Messages under the Tasks root) render just the
        heading.
    10. Only spaces and data-derived text: no box drawing, no decoration.
5.  Tests: assert on draw-op vectors (x, y, text, fg, styles) for
    representative states: centred uppercase headings; wrapping at 80 with
    indentation; colour cycling by depth; focused vs unfocused cursor styling;
    scroll keeping the cursor visible; collapsed vs expanded message text;
    empty messages pane. Build state/row literals inline (Architecture 6.3).
6.  Verification: your tests pass against the stub, then again against the real
    model once nav-01 delivers (`bb test`).
7.  Process (Architecture 7): post `ready -> started` now; commit only your
    owned paths; post a results message with status `delivered` when done. Do
    not set `done`. Questions go in this folder as `question` messages.

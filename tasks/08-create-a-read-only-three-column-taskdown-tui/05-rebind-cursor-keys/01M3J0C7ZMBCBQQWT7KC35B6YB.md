---
created: Mon 28 Sep 2026 - 04h 00m 01s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: keys-01
subject: 'Results: cursor keys rebound, sibling fallback'
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: keys-01
---

# Results: cursor keys rebound, sibling fallback added

1.  Both amendments implemented inside my four owned files only; the public
    API and data shapes are unchanged (additive change).
2.  Model (`src/task_down/model.clj`):
    1.  `move-cursor` gained `:next`/`:prev`: pure visual movement to the row
        below/above in flat pane row order, heading included, stepping over
        pruned siblings; noop past either end.
    2.  `:down`/`:up` keep sibling semantics but now fall back to the visual
        row when no next/previous sibling exists (second amendment): `}` at a
        subtree end lands on the displayed child/uncle/cousin row, `{` on a
        first child lands on its parent, heading-up and last-row-down stay
        noops.
    3.  The ancestor resets and the same-row noop apply to every direction,
        fallback moves included.
    4.  Docstrings updated: `move-cursor`, `vertical-cursor`, `peer-cursors`.
3.  Controller (`src/task_down/controller.clj`): key map is now `{` -> `:up`,
    `}` -> `:down`, `j` -> `:next`, `k` -> `:prev`; `h`/`l`, `gg`/`G`,
    `enter`/`escape`, `q` unchanged. The fallback lives in the pure model, not
    the key map. `key-actions` docstring updated.
4.  Tests:
    1.  Model: `:next`/`:prev` in task panes (heading transitions,
        pruned-tree gaps, last-row noop), messages pane, ancestor resets;
        every worked example of the fallback amendment; three pre-existing
        assertions that encoded the old no-fallback noops updated to the new
        behaviour.
    2.  Controller: full new key map through the fake Terminal, including
        `{`/`}` sibling steps and a `}` fallback with no sibling.
5.  Verification:
    1.  `cd scripts/task-down && bb test`: 62 tests, 308 assertions,
        0 failures, 0 errors (was 57/257).
    2.  `clj-kondo --lint src test`: 0 errors, 0 warnings.
    3.  Pod tty probe (dedicated tmux window, closed itself, verified gone):
        the lanterna pod delivers `{` and `}` as `java.lang.Character` `\{`
        and `\}` (also re-confirmed `j` `k` `q`); no workaround was needed.
    4.  Manual tty run against the real vault in a dedicated `tdprobe` tmux
        session at 250x50 (killed afterwards): `j`/`k` walked visually across
        parent, sibling and child rows; `}` on a subtree's last child fell
        back to the uncle row and `{` on a first child fell back to the
        parent, both observable through the listing/messages headings; `G`,
        `gg`, `enter`/`escape` behaved; `q` exited with `EXIT=0` and the shell
        afterwards reported a sane cooked terminal via `stty -a`.
    5.  Note: the shared session was resized to 183 columns mid-run, which
        clips the 244-column three-pane layout; that is environmental, the
        app re-read the size each turn and behaved correctly.
6.  File sizes: model.clj 236, controller.clj 106, model_test.clj 395,
    controller_test.clj 253; growth is the mandated coverage only.
7.  Commits, explicit pathspecs on my four files plus this task folder:
    `b27ce78` (rebind + `:next`/`:prev`), `e106229` (sibling fallback),
    `b78fe67` (started message), plus the commit carrying this message.
8.  Status: delivered, ready for review.

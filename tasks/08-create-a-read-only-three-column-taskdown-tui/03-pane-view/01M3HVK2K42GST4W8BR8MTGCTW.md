---
created: Mon 28 Sep 2026 - 02h 36m 22s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: view-01
subject: Pane view delivered
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: view-01
---

# Pane view delivered

1.  Deliverables

    1.  `scripts/task-down/src/task_down/view.clj`, 194 lines.
        1.  `(render state) -> vector of draw ops`, pure, no I/O, no atoms.
        2.  Requires only `clojure.string` and `task-down.model`.
        3.  Calls just three model fns: `rows`, `cursor-row-index`, `ancestor`.
    2.  `scripts/task-down/test/task_down/view_test.clj`, 236 lines.
        1.  18 tests, 48 assertions, all passing.
        2.  Rows are inline literals; `model/rows`, `model/cursor-row-index`
            and `model/ancestor` are replaced with them via `with-redefs`, so
            the suite pins the view alone and is unaffected by model changes.

2.  Contract compliance (Architecture 4.3)

    1.  Pane width 80, x offsets 1, 82, 163.
    2.  Headings uppercase and centred on row 0; content from row 1.
    3.  Word wrap at the pane width, indentation counting toward it, over-long
        words split at the width.
    4.  Task colour by generation depth, cycling `[:red :green :yellow :blue
        :magenta :cyan]` with depth 0 red.
    5.  Cursor row `#{:reverse}` when its pane holds focus, `#{:underline}`
        when it does not.
    6.  Vertical scroll derived, never stored: the smallest offset that keeps
        the whole cursor row visible, otherwise the top of the pane.
    7.  Collapsed message row: created datetime with the year token dropped,
        two spaces, subject.
    8.  Expanded message row: subject, author, created, a blank line, then the
        raw markdown body.
    9.  Empty panes render just their heading.
        1.  A pane the model leaves empty falls back to a heading built from
            `model/ancestor`, so the heading always shows.
    10. Only spaces and data-derived characters are drawn.
        1.  A test strips whitespace from every op and asserts each remaining
            character occurs in the source rows.
        2.  The same test asserts no box-drawing or block character appears.

3.  Interpretations the contract left open, each covered by a test

    1.  The heading is pinned to row 0 and never scrolls away.
        1.  4.3.2 puts headings on row 0 unconditionally, so content scrolls
            beneath it across rows 1 to rows-1.
        2.  A heading longer than the pane is cut at the pane width.
    2.  Cursor rows are padded with spaces to the full pane width.
        1.  One item then reads as one highlight even when it wraps, matching
            task spec 9.3, "the cursor is on the item, not on a specific line".
        2.  Padding is spaces only, so 4.3.9 holds and the gutter columns
            between panes stay empty.
    3.  Wrapped continuation lines repeat their row's indentation.
        1.  A hanging indent keeps the generation structure legible, which
            task spec 7.3 asks spacing to do.
    4.  Cursor styles replace a row's base styles.
        1.  A cursor row is exactly `#{:reverse}` or `#{:underline}`, the
            literal reading of 4.3.5; its foreground colour is unchanged.
    5.  A contributor-map author renders as name, harness and model joined by
        single spaces, for example `view-01 pi qwen/qwen3.8-max-0902`.
        1.  Both keyword and string frontmatter keys are accepted.
        2.  Real vault messages carrying `harness: pi` and `model: unknown`
            with no name render as `pi unknown`, which is faithful to the data.
    6.  Whitespace runs inside text survive wrapping.
        1.  Raw markdown bodies keep their `1.  ` double spaces and their
            nested indentation, so "raw" in 4.3.8 holds.
        2.  Only trailing whitespace at a wrap point is dropped.
    7.  Expanded message subject lines are white and bold, all other message
        lines `:default`, so bold carries hierarchy as 4.3.9 asks.
    8.  `render` reads `:state/size :rows` for the viewport and ignores
        `:cols`, because 4.3.2 fixes the layout at 244 columns.
        1.  A state with no `:state/size` fails loudly rather than drawing
            nothing.

4.  Verification

    1.  `bb test` in `scripts/task-down/`: 42 tests, 215 assertions, 0
        failures, 0 errors, exit 0. That is the view, model and vault suites
        together, so the view passes against nav-01's real model.
    2.  The view suite also passed earlier against the throwaway stub at
        `/tmp/view-stub-src`, which is now deleted and was never committed.
    3.  `clj-kondo --lint src/task_down/view.clj test/task_down/view_test.clj`:
        0 errors, 0 warnings.
    4.  End-to-end check: loaded the real `tasks/` vault with
        `vault/load-vault`, drove it through `model/initial-state`,
        `model/move-cursor` and `model/move-focus`, rendered with `view/render`
        and painted the ops into a text grid.
        1.  Parents pane pruned to branching tasks, listing pane showed the
            full subtree, both indented two columns per generation.
        2.  Messages pane showed collapsed datetimes without years and the
            newest message expanded with its wrapped body.
        3.  A four-row terminal scrolled the listing pane while its heading
            stayed pinned on row 0.

5.  Notes for the coordinator

    1.  `view.clj` is 194 lines, inside the ~200 limit. No function exceeds
        about a dozen lines, and there are no macros or multimethods.
    2.  `view_test.clj` is 236 lines, above the ~200 limit.
        1.  I merged the overlapping tests to get there from 26 tests.
        2.  Cutting further meant dropping a deliverable's coverage, so I kept
            it; tell me if you would rather I split or reduce it.
    3.  Public view API beyond `render`: `pane-width`, `pane-order`,
        `pane-offsets`, `depth-colours`, `heading-row`, `content-row`,
        `cursor-styles` and `wrap`. Everything else is private.
        1.  ctrl-01 may find `pane-width` and `pane-offsets` useful for sizing
            the terminal, but the controller needs none of them to draw.
    4.  A few test lines run to 83 columns; clj-kondo is configured without a
        line-length rule here, so I preferred the shorter file.

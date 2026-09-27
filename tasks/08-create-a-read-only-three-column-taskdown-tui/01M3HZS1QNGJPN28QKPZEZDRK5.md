---
created: Mon 28 Sep 2026 - 03h 49m 32s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: 'Amendment: sibling fallback for brace keys'
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: coordinator
---

# Amendment: sibling fallback for brace keys

1.  Supersedes the `:up`/`:down` semantics of the previous amendment
    (`01M3HYP4P8J4DKAZKAVQ2GAF9W`); the key map itself is unchanged.
2.  Directive (from the user): when there is no sibling to jump to, `{` and `}`
    fall back to visual movement - `}` takes you to the next item in the list
    (a child, an uncle or a cousin, whatever is displayed below), `{` to the
    previous one.
3.  Model, `move-cursor`:
    1.  `:down` - the next sibling when one exists; otherwise the next visual
        row (the `:next` behaviour). Noop only when neither exists, i.e. on the
        last row of the pane.
    2.  `:up` - the previous sibling when one exists; otherwise the previous
        visual row (the `:prev` behaviour). Noop only when neither exists, i.e.
        on the heading row.
    3.  `:next` and `:prev` remain pure visual row movement, unchanged.
    4.  The ancestor-change resets and the same-row noop rule apply to fallback
        moves exactly as they do to sibling moves.
4.  Controller: no change - `{` still maps to `:up` and `}` to `:down`. The
    fallback lives in the pure model, not in the key map.
5.  Worked examples (parents and listing panes):
    1.  `}` on a node that has both children and a next sibling: the sibling,
        skipping the subtree, as before.
    2.  `}` on a last child that has children: its first child (fallback).
    3.  `}` on a childless last child: the next visual row - an uncle or cousin
        (fallback).
    4.  `{` on a first child: its parent (fallback).
    5.  `{` on the heading: noop. `}` on the last row: noop.
6.  Messages pane: rows are flat there, so sibling and visual movement already
    coincide; behaviour is unchanged.

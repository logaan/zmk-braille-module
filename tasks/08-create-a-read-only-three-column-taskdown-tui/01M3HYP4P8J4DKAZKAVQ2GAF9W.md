---
created: Mon 28 Sep 2026 - 03h 30m 28s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: 'Amendment: cursor key bindings'
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: coordinator
---

# Amendment: cursor key bindings

1.  Supersedes the bindings in Architecture 4.2.5 and 4.5.3; everything else
    stands unchanged.
2.  Directive (from the user): `j`/`k` follow the visual order of the UI;
    sibling navigation moves to `{`/`}`.
3.  Model, `move-cursor` gains two directions:
    1.  `:next` - the row visually below the cursor row in that pane's `rows`
        (flat row order, heading row included); noop on the last row.
    2.  `:prev` - the row visually above; noop on the heading row (row 0).
    3.  `:up`/`:down` keep their sibling semantics unchanged.
    4.  The ancestor-change resets and the same-row noop rule apply to
        `:next`/`:prev` exactly as they do to `:up`/`:down`.
4.  Controller key map becomes:
    1.  `{` - sibling up (model `:up`).
    2.  `}` - sibling down (model `:down`).
    3.  `j` - visual down (model `:next`).
    4.  `k` - visual up (model `:prev`).
    5.  `h`/`l`, `gg`/`G`, `enter`/`escape`, `q` are unchanged.
5.  Vault, view and terminal layers are unaffected.

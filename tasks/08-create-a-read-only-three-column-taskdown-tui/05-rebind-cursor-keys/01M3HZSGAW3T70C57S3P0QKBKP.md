---
created: Mon 28 Sep 2026 - 03h 49m 47s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: 'Scope update: sibling fallback for braces'
cargo:
  type: task
  status: started
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: keys-01
---

# Scope update: sibling fallback for { and }

1.  The user added a requirement after you started. The parent folder has a new
    amendment message - `01M3HZS1QNGJPN28QKPZEZDRK5`, "Amendment: sibling
    fallback for brace keys". Read it before delivering; it supersedes the
    `:up`/`:down` semantics of the first amendment.
2.  Additional deliverables, all inside your four owned files:
    1.  `model/move-cursor`: `:down` falls back to the next visual row when
        there is no next sibling; `:up` falls back to the previous visual row
        when there is no previous sibling. `:next`/`:prev` stay pure visual.
        Resets and the same-row noop apply to fallback moves too.
    2.  Controller key map: unchanged from your current work (`{` `:up`, `}`
        `:down`, `j` `:next`, `k` `:prev`).
    3.  Model tests for the amendment's worked examples: `}` on a last child
        with children lands on its first child; `}` on a childless last child
        lands on the uncle/cousin row; `{` on a first child lands on the
        parent; `{` on the heading and `}` on the last row are noops; a
        fallback move that changes the ancestor still resets the panes below.
    4.  Update every docstring that describes `:up`/`:down`.
3.  Everything else in your assignment stands: verification steps, the tty
    check (now also exercise a `}` fallback at the end of a subtree), commit
    pathspecs, delivered message, probe-window and pane cleanup.
4.  If you have already committed the pre-fallback version, that is fine -
    extend it in a follow-up commit; do not rewrite history.

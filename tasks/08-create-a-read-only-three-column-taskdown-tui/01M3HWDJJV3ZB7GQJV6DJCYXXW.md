---
created: Mon 28 Sep 2026 - 02h 50m 51s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: 'Note: agents close their own tmux panes'
cargo:
  type: task
  status: started
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: coordinator
---

# Note: agents close their own tmux panes

1.  New process rule, effective immediately (supersedes any earlier instruction
    to keep a pi session open).
2.  When your work is complete - delivered message posted, files committed -
    your final action is to close your own tmux pane by running:
    `tmux kill-pane -t $TMUX_PANE`
3.  Do this LAST: never before your delivered message and commits are on disk.
4.  If you created extra tmux windows or panes for probing (for example a tty
    test window), kill those too before closing your own pane.
5.  If you are blocked or awaiting a coordinator answer, do NOT close your pane;
    stay available.

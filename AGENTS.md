# Agent notes

1.  If you've been asked to work with tasks
    1.  This project tracks tasks with TaskDown in `./tasks/`.
    2.  Use `./scripts/td` to work with them.
    3.  Run `./scripts/td system` for the TaskDown reference.
    4.  TaskDown is maintained separately in
        `~/code/personal/babashka/taskdown`. The scripts here are compatibility
        launchers, not its implementation. Set `TASKDOWN_HOME` if that checkout
        is elsewhere. TaskDown's own development tasks moved there;
        braille-module tasks remain here.
2.  There may be other agents working in the same worktree.
3.  Commit your own work as you go.
4.  Before each commit, update `references/braille-cheatsheet.typ` with new
    information and run `typst compile references/braille-cheatsheet.typ` to
    rebuild its PDF.

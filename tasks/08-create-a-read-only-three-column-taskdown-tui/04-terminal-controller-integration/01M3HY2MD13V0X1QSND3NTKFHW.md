---
created: Mon 28 Sep 2026 - 03h 19m 49s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: ctrl-01
subject: 'Results: terminal layer, controller and integration'
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: ctrl-01
---

# Results

1.  Delivered the terminal layer, the controller and the integration in
    `scripts/task-down/`. The three-pane UI runs end to end against the real
    vault, and the whole suite is green.

2.  Files created, all owned by ctrl-01, nothing else touched:

    1.  `src/task_down/terminal.clj` - 114 lines: the `Terminal` protocol and
        the `lanterna-terminal` record (Architecture 4.4).
    2.  `src/task_down/controller.clj` - 103 lines: the one global atom, the
        key map and the render / read / apply loop (Architecture 4.5).
    3.  `src/task_down/main.clj` - 18 lines: `-main`, optional first arg is the
        vault dir, default `tasks` (Architecture 4.6).
    4.  `test/task_down/controller_test.clj` - 229 lines: 15 tests,
        42 assertions, all driven through a fake Terminal record.
    5.  `task-down` - 24-line `#!/usr/bin/env bb` launcher, executable, runs
        from the repo root and from any other cwd.
    6.  `bb.edn` - **not** extended. Its `test` task already globs every
        `test/**/*_test.clj`, so `controller_test.clj` was picked up unchanged,
        and the launcher is the documented entry point, so no run task was
        needed. I own the file but left it alone to keep vault-01's work intact.

3.  Public API

    1.  `task-down.terminal`: the `Terminal` protocol with `start!`, `stop!`,
        `draw!`, `read-key!` and `size!`, exactly as contracted, plus
        `(new-lanterna-terminal) -> Terminal`, the factory that loads the pod
        and creates the screen. The record is `lanterna-terminal`, whose single
        field is the opaque pod screen handle.
    2.  `task-down.controller`: `(start! vault-dir)` per Architecture 4.5.1,
        `(show! terminal vault)` (the injectable half of `start!`, so tests
        drive the loop with a fake Terminal), `app-state` (the single atom) and
        `quit-key`.
    3.  `task-down.main`: `-main` and `default-vault-dir`.

4.  Lanterna: it worked, the ANSI fallback was not needed

    1.  Every one of your caveats in `01M3HVW8AF3D8KJ7KGHNJPZX1S` checked out
        against a real tty, and each is recorded in the `terminal.clj` ns
        docstring:
        1.  There is indeed no `clear` var, so `draw!` blanks each frame with
            one row of spaces per screen row before painting the ops. Lanterna
            buffers and diffs on `redraw`, so the blank costs pod calls, not
            flicker.
        2.  Handles come back as `#:pod.babashka.lanterna{:terminal "uuid"}` /
            `{:screen "uuid"}`; they are stored in the record and passed back
            whole, never destructured.
        3.  `get-size` returns `[cols rows]`, converted to `{:rows r :cols c}`.
        4.  `get-key-blocking` returns a Character for printable keys and a
            Keyword for special ones. `read-key!` normalises characters,
            `:enter` and `:escape` to `{:key k}` and returns nil for everything
            else (`:up`, `:backspace`, ...), which the controller ignores.
        5.  `put-string` opts are exactly the draw-op keywords: all nine
            colours plus `:bold`, `:underline` and `:reverse` were verified on
            screen, and view ops map onto them with no translation table.
    2.  Two things your note did not cover, found by probing:
        1.  `get-terminal` accepts `:auto`, `:text`, `:swing` and `:cygwin` but
            **not** `:unix` (`No matching clause`). I pass `:text {}`
            explicitly: `:auto` falls through to
            `TerminalFacade/createTerminal`, which opens a Swing window whenever
            the JVM is not headless, so `:text` removes that risk.
        2.  Boot sequence confirmed as you predicted: `load-pod`, require
            nothing by hand, `(get-terminal :text {})`, `(terminal-screen t)`,
            `(start screen)`, `stop` in a `finally`. `requiring-resolve` finds
            the pod vars once the pod is loaded, and `load-pod` is idempotent.
    3.  Drawing past the right edge or below the bottom is silently clipped,
        not an error, so a terminal narrower than 244 columns degrades instead
        of crashing (verified at 100 columns).

5.  Decisions where the contract left room

    1.  The pod is loaded inside a `delay`, not at require time, so
        `task-down.terminal` can be required for the protocol alone. That is
        what keeps the test suite off the pod binary: `bb test` never loads it.
    2.  `start!` reads the vault *before* the terminal is taken over, so a bad
        directory reports on an ordinary shell (`NoSuchFileException`, exit 1)
        instead of stranding the user in a raw-mode screen. `show!` still wraps
        the loop in `try/finally`, so `stop!` runs even when the loop throws.
    3.  The loop is draw -> read -> apply, so `q` never writes state: the atom
        always holds a real state map and ends on the state the user last saw.
    4.  `gg` uses `:state/pending-key` exactly as modelled: `\g` sets `:g`, a
        second `\g` jumps to the top, and any other key - including one the
        controller does not know - clears it, so `g x g` is not a jump.
    5.  An ignored key still costs one redraw. Lanterna only writes changed
        cells on refresh, so a repeated frame is free at the tty and the loop
        stays a single unconditional path.
    6.  `run!` is called `show!` because `run!` shadows `clojure.core/run!`
        (clj-kondo warning). `start!` keeps the contracted name.
    7.  The launcher stays in-process: it puts its own `src` on the classpath
        with `babashka.classpath/add-classpath` and calls `-main`. Each step is
        its own top-level form, because SCI analyses a whole `let` before
        evaluating it, so a runtime `require` alias inside one `let` body is
        not visible to the next expression in it.
    8.  Babashka rejects a docstring in `defonce` and in `defrecord` (both
        throw `String cannot be cast to Named` or an arity error), so the
        record's documentation is a comment above it and the atom's is
        metadata.

6.  Verification

    1.  `cd scripts/task-down && bb test` -> `Ran 57 tests containing 257
        assertions. 0 failures, 0 errors.`, exit 0. That is the 42 pre-existing
        tests plus my 15 (42 assertions).
    2.  `controller_test.clj` alone -> 15 tests, 42 assertions, 0 failures.
    3.  `clj-kondo --lint src test` -> 0 errors, 0 warnings.
    4.  Manual verification in a real tty. My own pane has no tty for child
        processes, so I used a dedicated 250x42 tmux window in this session
        (killed afterwards, per the new process rule) and drove it with
        `send-keys` / `capture-pane -e`:
        1.  `./scripts/task-down/task-down` from the repo root: three panes,
            three centred `TASKS` headings, messages pane empty because the
            root has none.
        2.  Colours by generation verified in the escape codes: depth 0
            `ESC[31m` red, depth 1 `ESC[32m` green, depth 2 `ESC[33m` yellow,
            two columns of indent per generation, headings `ESC[1m ESC[37m`.
        3.  Cursor styling verified: `ESC[7m` reverse on the focused pane's
            row, `ESC[4m` underline on the same row in the other two.
        4.  `l` on the parents cursor moved the listing heading to
            `08 CREATE A READ ONLY THREE COLUMN TASKDOWN TUI` and filled the
            messages pane with that task's messages; `j`/`k` stepped siblings;
            `h`/`l` climbed and descended; `escape`/`enter` cycled focus
            listing -> parents -> messages.
        5.  Messages: collapsed rows read `Mon 28 Sep - 00h 07m 57s  Create a
            read-only three-pane TaskDown TUI` (year dropped, two spaces) and
            the newest was expanded to subject, author
            (`coordinator pi qwen/qwen3.8-max-0902`), created, blank line, raw
            body. `k` in the messages pane moved the highlight while the newest
            stayed expanded.
        6.  `G` jumped to the last row and scrolled it into view with the
            heading pinned on row 0; `gg` returned to the top.
        7.  Stale glyphs: after `l` (messages pane full) then `h` (root, no
            messages) every row below the heading was blank - the manual
            blanking works.
        8.  Resize: shrinking the window from 41 to 16 rows mid-session changed
            the next frame's viewport, proving the size is re-read each turn.
        9.  Quit: `q` left exit status 0, `stty -a` afterwards reports
            `icanon ... echo`, and the scrollback is ordinary shell text - the
            terminal is restored.
        10. Entry points: `./scripts/task-down/task-down <dir>` from the repo
            root, the same launcher from `/tmp` with an absolute vault path,
            and `bb -m task-down.main <dir>` from `scripts/task-down/` all
            render the requested vault; a nonexistent dir exits 1 on a normal
            shell.
    5.  Frame cost measured in the 250x41 window: vault load 23ms, pod plus
        screen creation 157ms at startup, then per frame `view/render` 1ms and
        `draw!` about 87ms for 41 blanking rows plus 56 ops, of which the
        blanking is 37ms and the `redraw` itself 1-3ms. The pod round trip
        dominates at roughly 0.9ms per call; at 250x15 the same frame is 44ms.
        That is 11-22 redraws a second on a keypress-driven UI, which feels
        immediate, so I kept the simple unconditional blanking rather than
        caching the previous frame in a second atom.

7.  Engineering rules held: no function body exceeds eight lines (the longest
    is `apply-action` at seven; the pod var map in `terminal.clj` is eleven
    lines of data in a `delay`); every file is inside the ~200-line guideline
    except `controller_test.clj` at 229, where cutting further meant dropping
    a deliverable's coverage - tell me if you would rather I split it; no
    macros defined, no multimethods; one protocol plus two records for the
    polymorphism; exactly one mutable atom in the app
    (`controller/app-state` - the atoms inside the test doubles are test
    scaffolding, and the pod `delay` is immutable).

8.  Nothing needed changing in vault-01's, nav-01's or view-01's files, and I
    found no bugs in them: the integration ran against their delivered code
    unmodified. No open questions.

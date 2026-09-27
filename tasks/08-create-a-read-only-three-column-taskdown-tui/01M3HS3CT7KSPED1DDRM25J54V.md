---
created: Mon 28 Sep 2026 - 01h 52m 51s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: 'Architecture: MVC with Lanterna pod'
cargo:
  type: task
  status: started
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: coordinator
---

# Architecture: three-pane TaskDown TUI

1.  Decision: terminal library

    1.  Use the Babashka pod `org.babashka/lanterna` version `0.0.1`.
    2.  Verified: it loads on this machine (arm64 macOS via Rosetta) and the
        screen API exposes everything needed:
        1.  `screen/put-string` with `{:fg colour :bg colour :styles #{:bold ...}}`.
        2.  `screen/get-key-blocking` for input.
        3.  `screen/get-size` for terminal dimensions.
    3.  Fallback plan: if Lanterna proves unusable in integration, implement a
        raw-ANSI terminal (escape codes + `stty` raw mode) behind the same
        `Terminal` protocol. Model and view are unaffected either way.
    4.  Load the pod with:
        `(pods/load-pod 'org.babashka/lanterna "0.0.1")` then require
        `pod.babashka.lanterna.screen`.

2.  Layout

    1.  Project root: `scripts/task-down/`.
    2.  Files and their single owner (never edit a file you do not own):
        1.  `bb.edn` - vault-01 (creates), ctrl-01 (may extend).
        2.  `src/task_down/vault.clj` - vault-01.
        3.  `src/task_down/model.clj` - nav-01.
        4.  `src/task_down/view.clj` - view-01.
        5.  `src/task_down/terminal.clj` - ctrl-01.
        6.  `src/task_down/controller.clj` - ctrl-01.
        7.  `src/task_down/main.clj` - ctrl-01.
        8.  `task-down` launcher script - ctrl-01.
        9.  `test/task_down/vault_test.clj` + `test/fixtures/**` - vault-01.
        10. `test/task_down/model_test.clj` - nav-01.
        11. `test/task_down/view_test.clj` - view-01.
        12. `test/task_down/controller_test.clj` - ctrl-01.
    3.  Namespaces: `task-down.vault`, `task-down.model`, `task-down.view`,
        `task-down.terminal`, `task-down.controller`, `task-down.main`.

3.  Data shapes (the shared contract; use exactly these)

    1.  Task node (the root uses the same shape):
        ```clojure
        {:task/name "01 Bake potatoes"   ; folder name, leading "NN-" kept, hyphens to spaces
         :task/path "tasks/01-bake-potatoes" ; vault-relative path; root is "tasks"
         :task/children [...]            ; vector of task nodes, sorted by folder name
         :task/messages [...]}           ; vector of message maps, sorted by filename ascending
        ```
    2.  Root node: `:task/name` is `"Tasks"`, `:task/messages` is `[]`.
    3.  Message map:
        ```clojure
        {:msg/id "01M3HK39RR353S0DQZRQBBAFFJ" ; filename without ".md"
         :msg/path "tasks/01-x/01M3....md"     ; vault-relative
         :msg/created "Mon 28 Sep 2026 - 00h 07m 57s" ; raw string from frontmatter
         :msg/author <parsed frontmatter author value>
         :msg/subject "..."
         :msg/cargo {...}
         :msg/body "..."}                      ; markdown body after frontmatter, trimmed
        ```
        1.  Frontmatter is YAML between the first two `---` lines.
            Parse with `clj-yaml.core/parse-string` (bb built-in).
    4.  App state (single atom, all values immutable):
        ```clojure
        {:state/vault <root node>
         :state/focus :parents | :listing | :messages
         :state/cursors {:parents :heading | [0 2]      ; index path into the vault tree
                          :listing :heading | [0 2]
                          :messages "01M3..." | :heading} ; message id
         :state/size {:rows 24 :cols 244}
         :state/pending-key nil | :g}                    ; multi-key "gg" support
        ```
    5.  Row (what panes display and cursors address):
        ```clojure
        {:row/kind :heading | :task | :message
         :row/node <node>        ; heading and task rows
         :row/message <msg>      ; message rows
         :row/path [0 2]         ; task rows: index path into the vault tree
         :row/depth 0            ; task rows: generation under the pane ancestor
         :row/expanded? false}   ; message rows only
        ```
    6.  Draw op (view output, controller input):
        ```clojure
        {:x 0 :y 0 :text "TASKS" :fg :white :styles #{:bold}}
        ```
        1.  Colours: `:black :white :red :green :yellow :blue :magenta :cyan :default`.
        2.  Styles: `#{:bold :underline :reverse}`.

4.  Function contracts (pure unless marked)

    1.  `task-down.vault` (owned by vault-01):
        1.  `(load-vault dir) -> root node` (impure: reads the filesystem).
        2.  `(parse-message path text) -> message` (pure; text includes frontmatter).
        3.  Keep filesystem reading isolated; parsing logic pure and unit-testable.
    2.  `task-down.model` (owned by nav-01):
        1.  `(initial-state vault size) -> state` (cursors at starting positions).
        2.  `(ancestor state pane) -> node`
            1.  `:parents` -> root.
            2.  `:listing` -> node under the parents cursor; heading -> root.
            3.  `:messages` -> node under the listing cursor; heading -> the
                listing pane ancestor itself.
        3.  `(rows state pane) -> vector of row maps` (row 0 is always the heading).
            1.  `:parents` shows only tasks that have child tasks (recursively
                pruned; a kept task shows only its kept children).
            2.  `:listing` shows the full subtree under its ancestor.
            3.  `:messages` shows heading + that task messages, oldest first;
                `:row/expanded?` true for the most recent message only.
        4.  `(cursor-row-index state pane) -> int`.
        5.  `(move-cursor state pane direction) -> state`, direction in
            `:up :down :left :right :top :bottom`.
            1.  up/down = previous/next sibling (messages: previous/next message;
                heading row reachable from the first item via up).
            2.  left = parent (from depth 0 task -> heading; heading -> noop).
            3.  right = first child (heading -> first row; no child -> noop).
            4.  top/bottom = first/last row.
            5.  Missing target -> unchanged state (noop).
            6.  Moving the parents cursor resets the listing cursor to `:heading`;
                moving the listing cursor resets the messages cursor to the most
                recent message (or `:heading` when none).
        6.  `(move-focus state direction) -> state`, direction `:left :right`,
            wrapping around the three panes.
    3.  `task-down.view` (owned by view-01):
        1.  `(render state) -> vector of draw ops`.
        2.  Layout: pane width 80; pane x offsets 1, 82, 163; headings uppercase
            and centred on row 0; content starts at row 1.
        3.  Wrap text at the pane width; indentation counts toward the width.
        4.  Task name colour by generation depth, cycling through
            `[:red :green :yellow :blue :magenta :cyan]` (depth 0 = red).
        5.  Cursor row: `#{:reverse}` when the pane is focused, `#{:underline}`
            when it is not (the dimmed look).
        6.  Scroll only vertically, derived (not stored): keep the cursor row
            visible, otherwise show from the top.
        7.  Collapsed message row text: created datetime with the year token
            removed, then two spaces, then the subject. Example:
            `Mon 28 Sep - 00h 07m 57s  Create a read-only three-pane TaskDown TUI`.
        8.  Expanded message rows: subject, author, created (one line each, from
            message data only), a blank line, then the raw markdown body.
        9.  No decorative characters anywhere: only spaces and content from the
            data model. Colour, bold and uppercase carry the hierarchy.
        10. `render` may call pure `task-down.model` fns (rows,
            cursor-row-index, ancestor); it must not require vault, terminal or
            controller.
    4.  `task-down.terminal` (owned by ctrl-01):
        1.  Protocol:
            ```clojure
            (defprotocol Terminal
              (start! [this]) (stop! [this])
              (draw! [this ops])        ; clear screen, put-strings, redraw
              (read-key! [this])        ; -> {:key \j} | {:key :enter} | {:key :escape}
              (size! [this]))           ; -> {:rows n :cols n}
            ```
        2.  Record `lanterna-terminal` implements it.
    5.  `task-down.controller` (owned by ctrl-01):
        1.  `(start! vault-dir) -> nil` (impure; returns when the user quits).
        2.  One global atom holding the state map; loop: render -> read key ->
            apply to state via `task-down.model` fns -> repeat.
        3.  Keys: `q` quit; `j k h l` move the cursor in the focused pane;
            `g g` jump to top; `G` jump to bottom; `enter` focus left;
            `escape` focus right. Re-read terminal size each iteration.
    6.  `task-down.main` (owned by ctrl-01): entry point; optional first arg is
        the vault dir, default `tasks`.

5.  Engineering rules

    1.  Models and views are pure: no I/O, no atom access, no side effects.
    2.  Exactly one mutable atom in the whole app (in the controller).
    3.  Functions at most about a dozen lines; files at most about 200 lines.
    4.  No multimethods, no macros.
    5.  Records and protocols where polymorphism is warranted (the Terminal).
    6.  Clojure style: kebab-case, docstrings on public fns, `let` over nesting.

6.  Testing

    1.  `clojure.test` only. Test pyramid: the bulk of tests target pure model
        and view code with literal data; vault tests use small fixture vaults;
        controller tests use a fake Terminal record.
    2.  `bb.edn` task `test` runs every `test/**/*_test.clj` and exits non-zero
        on failure. Until bb.edn exists, run tests with:
        ```bash
        bb --classpath src:test -e "(require '[clojure.test :as t] 'task-down.model-test)(t/run-tests 'task-down.model-test)"
        ```
    3.  Fixtures live under `scripts/task-down/test/fixtures/` and are owned by
        vault-01. Other agents must not read fixtures from disk in their tests;
        build node/message literals inline instead.

7.  Process

    1.  Each agent reads all messages in
        `tasks/08-create-a-read-only-three-column-taskdown-tui/` before working,
        then its own subtask folder.
    2.  Status lifecycle per TaskDown: post `ready -> started` when beginning;
        post a results message with status `delivered` when complete. Never set
        `done` yourself; the coordinator reviews.
    3.  Commit your own files as you go. Stage only paths you own plus your own
        task folder, e.g.:
        `git add scripts/task-down/src/task_down/vault.clj tasks/08-*/01-vault-loader && git commit -m "..."`.
        Never `git add -A` or `git add .`: other agents work in the same tree.
    4.  Write messages with `./scripts/td message new ...` (see `--help`); it
        generates valid ULIDs and frontmatter. Validate with
        `./scripts/td message validate`.
    5.  Questions to the coordinator go in your subtask as a `question` message;
        check for answers before delivering.

---
created: Mon 28 Sep 2026 - 03h 24m 47s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: 'Results: three-pane TaskDown TUI'
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: coordinator
---

# Results: three-pane TaskDown TUI

1.  The read-only three-pane TaskDown TUI is implemented in
    `scripts/task-down/` and verified against the real vault.
2.  Run it: `./scripts/task-down/task-down [vault-dir]` (default `tasks`).
    1.  Keys: `q` quit; `j`/`k` siblings; `h`/`l` parent/child (`l` descends
        from a heading); `enter`/`escape` cycle focus left/right; `gg` top;
        `G` bottom.
3.  Architecture: message `01M3HS3CT7KSPED1DDRM25J54V` in this folder.
    1.  MVC: pure model and view, impure controller, one atom, immutable data.
    2.  Terminal: `Terminal` protocol over the `org.babashka/lanterna` `0.0.1`
        pod (works on arm64 macOS via Rosetta); the pre-approved ANSI fallback
        was never needed.
4.  Code: six source files - vault 138, model 220, view 194, terminal 114,
    controller 103, main 18 lines - plus a 24-line launcher and `bb.edn`.
5.  Tests: four suites, 57 tests, 257 assertions, all green; clj-kondo clean;
    pyramid honoured (pure model/view bulk, fixture-driven vault, fake-terminal
    controller).
6.  Built by four `qwen/qwen3.8-max-0902` subagents in tmux panes, coordinated
    through this task tree; each subtask reviewed and done, each agent closed
    its own pane:
    1.  vault-01: loader, parser, fixtures, test runner (`01-vault-loader`).
    2.  nav-01: state, cursors, ancestors, navigation (`02-navigation-model`).
    3.  view-01: layout, wrapping, colours, scroll, messages (`03-pane-view`).
    4.  ctrl-01: terminal, controller, main, launcher, integration
        (`04-terminal-controller-integration`).
7.  Spec conformance highlights: 80-column panes at offsets 1/82/163 with blank
    gutters; uppercase centred headings; parents pane pruned to branching
    tasks; six deterministic generation colours; wrapping with hanging indent;
    cursor reverse-video focused / underline unfocused; collapsed messages show
    year-less datetime plus subject with the newest expanded; derived scroll
    with pinned heading; no decorative characters anywhere.
8.  Verification: unit, fixture and fake-terminal tests; ctrl-01's in-tty
    manual pass (colours via escape codes, stale-glyph blanking, resize, quit
    with `stty` check); my independent smoke test in a fresh session (render,
    navigate, `q` exit 0, terminal restored); frame cost ~87ms at 250x41,
    keypress-driven.
9.  Notes and deviations:
    1.  Two test files exceed the ~200-line guideline (`view_test` 236,
        `controller_test` 229); both agents merged tests rather than drop
        coverage. `model.clj` at 220 was accepted for docstring density. All
        other files are inside the guideline.
    2.  `td task new` has a `--name` flag collision (folder name vs author
        agent name): passing both silently named the four subtask folders
        `NN-coordinator` and dropped the author name. I renamed the folders and
        fixed the metadata by hand. Worth fixing in `scripts/td` (for example a
        separate `--folder-name`).
    3.  vault-01's first messages recorded the settings default model
        (`gpt-6-sol`) instead of its runtime model; corrected, and the identity
        note (`01M3HTESKGFYMZPG6SGTB9RYSB`) now governs all agents. No gpt
        model was ever run.
    4.  ctrl-01's commit `1d5f385` accidentally included a rename another
        agent had staged in the shared index (content identical, later
        superseded by that agent's own commits). Caution recorded in its
        review; no damage.
    5.  Interpretations the spec left open are documented per agent in the
        subtask results messages: author rendering (`name harness model`),
        pinned heading, full-width cursor padding, focus wrapping, initial
        focus `:parents`, heading-as-parent in the messages pane, same-row
        noop, `:text` terminal kind.
10. Status: delivered, ready for your review. Set `done` when satisfied, or
    tell me what to change and I will task an agent with it.

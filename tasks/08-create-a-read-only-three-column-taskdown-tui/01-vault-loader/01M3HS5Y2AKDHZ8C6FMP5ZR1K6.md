---
created: Mon 28 Sep 2026 - 01h 54m 14s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: Build the vault loader and message parser
cargo:
  type: task
  status: ready
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: vault-01
---

# Build the vault loader and message parser

1.  Read every message in the parent task folder
    `tasks/08-create-a-read-only-three-column-taskdown-tui/` first. The
    Architecture message is the binding contract: data shapes (3.1-3.3), your
    function contract (4.1), engineering rules (5), testing (6), process (7).
2.  You own ONLY these paths (never touch anything else):
    1.  `scripts/task-down/bb.edn`
    2.  `scripts/task-down/src/task_down/vault.clj`
    3.  `scripts/task-down/test/task_down/vault_test.clj`
    4.  `scripts/task-down/test/fixtures/**`
3.  Deliverables:
    1.  `load-vault` (impure, thin): reads a tasks directory into the root node
        shape. Children sorted by folder name; messages sorted by filename
        ascending. Root node: name `"Tasks"`, path `"tasks"` (the dir given),
        messages `[]`. In a task dir: subdirectories are subtasks, `*.md` files
        are messages; ignore everything else (e.g. `.DS_Store`).
    2.  `parse-message` (pure): splits YAML frontmatter (between the first two
        `---` lines) from the body; returns the message shape with `:msg/body`
        trimmed. Use `clj-yaml.core` (bb built-in).
    3.  Display names: folder name with ALL hyphens replaced by spaces
        (`01-Bake-potatoes` -> `01 Bake potatoes`).
    4.  `bb.edn`: `:paths ["src" "test"]` plus a `test` task that runs every
        `test/**/*_test.clj` and exits non-zero on failure (Architecture 6.2).
    5.  Fixtures: a small fake vault under `test/fixtures/` with at least two
        top-level tasks, one nested subtask containing its own message, an
        agent-authored message and a message with a multi-line body.
    6.  Tests: pure tests for `parse-message` (frontmatter fields, body
        trimming, author shapes) and fixture-driven tests for `load-vault`
        (tree shape, both orderings, display names, ignoring junk files).
4.  Verification before delivering:
    1.  `cd scripts/task-down && bb test` passes.
    2.  Smoke test against the real vault prints 5 without error:
        `cd scripts/task-down && bb -e "(require '[task-down.vault :as v]) (prn (count (:task/children (v/load-vault \"../../tasks\"))))"`
5.  Process (Architecture 7): post `ready -> started` now; commit only your
    owned paths as you go; post a results message with status `delivered` when
    done. Do not set `done`. Post questions as `question` messages in this
    folder and wait for the coordinator answer before delivering if blocked.

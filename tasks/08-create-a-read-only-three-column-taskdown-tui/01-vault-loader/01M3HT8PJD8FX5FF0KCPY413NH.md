---
created: Mon 28 Sep 2026 - 02h 13m 14s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: vault-01
subject: 'Results: vault loader and message parser'
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: vault-01
---

# Results

1.  Delivered the vault loader and message parser in `scripts/task-down/`.

2.  Files created, all owned by vault-01, nothing else touched:
    1.  `bb.edn` - `:paths ["src" "test"]` plus a `test` task that globs every
        `test/**/*_test.clj` (top level and nested), requires each namespace,
        runs `clojure.test/run-tests` and exits with `fail + error`.
    2.  `src/task_down/vault.clj` - 137 lines, namespace `task-down.vault`.
    3.  `test/task_down/vault_test.clj` - 12 tests, 50 assertions.
    4.  `test/fixtures/vault/**` - the fake vault.

3.  Public API, exactly Architecture 4.1:
    1.  `(load-vault dir) -> root node`, impure and thin.
    2.  `(parse-message path text) -> message`, pure.
    3.  `(split-frontmatter text) -> [yaml body]`, pure; exported because it is
        the interesting half of the parser and is tested directly.
    4.  `(display-name folder-name) -> "01 bake potatoes"`, pure; every hyphen
        becomes a space and the leading `NN-` is kept.
    5.  `root-name` is `"Tasks"`.

4.  Contract notes for nav-01, view-01 and ctrl-01:
    1.  `:msg/author` and `:msg/cargo` are the frontmatter values exactly as
        `clj-yaml.core/parse-string` returns them: nested keys are keywords and
        the maps are `flatland.ordered.map.OrderedMap`, so YAML key order
        survives into the view.
    2.  `(= {:type "task" :status "ready"} (:msg/cargo msg))` is `true`, so
        tests may compare against plain map literals.
    3.  `(map? (:msg/cargo msg))` is `false` for those ordered maps. Branch on
        `(some-> msg :msg/cargo :type)` rather than on `map?`.
    4.  A user author is the plain string `"user"`, not a map.
    5.  `:task/path` and `:msg/path` are strings built by joining the `dir`
        given to `load-vault` with the folder and file names, so
        `(load-vault "tasks")` yields `"tasks"` and
        `"tasks/01-x/01M3HK39RR353S0DQZRQBBAFFJ.md"`.
    6.  A file with no frontmatter parses to nil metadata with all of its text
        as the body instead of throwing, so one odd file cannot break the
        viewer.

5.  Loader rules:
    1.  Children sorted by folder name; messages sorted by filename ascending.
    2.  In a task dir, subdirectories are subtasks and `*.md` files are
        messages.
    3.  Everything else is ignored: non-`.md` files such as `notes.txt`, and any
        dot-prefixed entry such as `.DS_Store`, `.hidden/` or `.gitkeep`.
    4.  An empty task folder still loads, as a node with `[]` children and `[]`
        messages.

6.  Fixtures under `test/fixtures/vault/`:
    1.  Two top-level tasks, `01-bake-potatoes/` and `02-write-the-report/`.
    2.  A nested subtask `01-bake-potatoes/02-choose-a-variety/` holding its own
        message.
    3.  Agent-authored messages (`baker-01`, `researcher-01`) alongside user
        ones, covering both author shapes.
    4.  A multi-line body with a nested numbered list, and a body containing a
        `---` rule that must not be read as the closing fence.
    5.  Junk to ignore: `.DS_Store`, `notes.txt`, `.hidden/01M3FIXTUREH....md`.
    6.  An empty task folder, `02-write-the-report/01-collect-the-numbers/`.
    7.  `vault/.DS_Store` needed `git add -f` because the repo `.gitignore`
        ignores `.DS_Store` at any depth; the empty folder uses a tracked
        `.gitkeep` because git cannot store an empty directory.

7.  Verification, all passed:
    1.  `cd scripts/task-down && bb test` -> `Ran 23 tests containing 159
        assertions. 0 failures, 0 errors.` exit 0. The 23 include nav-01's
        `model-test`, which landed while I worked; `vault-test` alone is 12
        tests and 50 assertions.
    2.  The smoke test prints `5` and exits 0:

        ```bash
        bb -e "(require '[task-down.vault :as v]) (prn (count (:task/children (v/load-vault \"../../tasks\"))))"
        ```
    3.  `bb test` exit codes checked in a throwaway project: 0 when all pass, 1
        when a test fails, 1 with a message when `test/` holds no test files.
    4.  `clj-kondo --lint` on `vault.clj` and `vault_test.clj`: 0 errors,
        0 warnings.

8.  Engineering rules held: the longest fn body in `vault.clj` is 9 lines; no
    macros defined and no multimethods; only `load-vault` and the private
    `load-task`, `load-message`, `child-dirs` and `message-files` helpers touch
    the filesystem.

9.  No open questions.

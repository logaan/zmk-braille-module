---
created: Mon 28 Sep 2026 - 03h 32m 35s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: Fix the overloaded --name flag in td task new
cargo:
  type: task
  status: ready
---

# Fix the overloaded --name flag in td task new

1.  Summary

    1.  In `scripts/td`, the `--name` flag means "Author agent name" in the
        shared option spec but "Folder name" in `td task new`, and `task new`
        drops it before building the message.
    2.  Result: `td task new` has NO way to set the author agent name, and
        passing `--name` twice (folder + author, as the help for `message new`
        trains you to do) silently renames the folder to the last value.

2.  Observed behaviour (real incident, task 08 subtask creation)

    1.  Command:
        `td task new --parent 08-... --name vault-loader --subject "..." --status ready --worker agent --worker-name vault-01 --author agent --harness pi --model qwen/qwen3.8-max-0902 --name coordinator --body-file ...`
    2.  Expected: folder `01-vault-loader/`, author block with
        `name: coordinator`.
    3.  Actual: folder `01-coordinator/` (the second `--name` won; babashka.cli
        is last-wins with no duplicate-flag error) and the author frontmatter
        had no `name:` key at all.
    4.  All four subtask folders were created as `NN-coordinator`; I renamed
        them with `mv` and patched `name: coordinator` into the frontmatter by
        hand.

3.  Root cause pointers in `scripts/td`

    1.  Line ~281, `new-option-spec`: `[:name {... :desc "Author agent name"}]`.
    2.  Line ~362, `new-flags-data`: destructures the author name from `:name`
        (`agent-name :name`), and line ~368 validates `--name` requires
        `--author agent`.
    3.  Line ~621, `task-new-option-spec`: prepends its own
        `[:name {... :desc "Folder name: lowercase kebab-case ..."}]` and
        filters the shared spec (which excludes the author `:name` from the
        `task new` help, so the overload is invisible there).
    4.  Line ~696-698, `run-task-new`: uses `:name` as the folder name, then
        `(dissoc opts :parent :name)` before calling `new-flags-data`, so the
        author name can never reach the message.

4.  Impact

    1.  Silent data loss: contributor `name` (system spec 3.2.3) missing from
        agent-authored task messages; `td message validate` passes because the
        field is optional, so nothing flags it.
    2.  Silent misnaming: task folders named after whatever value landed last
        in `--name`.
    3.  Agents automating `td task new` (the intended workflow) hit this on
        their first subtask creation.

5.  Suggested fix (pick one, keep `message new` working)

    1.  Preferred: give the folder its own flag, e.g. `--folder-name`, and let
        `--name` mean "Author agent name" consistently in both `task new` and
        `message new`; update help, epilog and examples (the epilog currently
        documents `--name` as the folder override).
    2.  Alternative: add `--author-name` for `task new` and keep `--name` as
        the folder flag, documenting the difference in both helps.
    3.  Either way: `run-task-new` must stop dissocing the author name, and the
        two meanings must never share one flag.

6.  Regression tests (extend `scripts/test-td`)

    1.  `task new` with `--author agent --name X` writes `author.name: X`.
    2.  The folder-name flag sets the folder; the subject-derived default still
        applies when it is absent.
    3.  Passing both flags in one command produces the intended folder AND the
        intended author name.
    4.  `message new` author-name behaviour is unchanged.

7.  Current workaround for agents: create the task with the folder flag only,
    then patch `name:` into the author block, or write the message file
    directly and validate with `td message validate`.

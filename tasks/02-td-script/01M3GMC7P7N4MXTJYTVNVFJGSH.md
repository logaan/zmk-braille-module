---
author: user
cargo:
  status: draft
  type: task
created: Sun 27 Sep 15h11
subject: TaskDown CLI
---

# TaskDown CLI

1.  Create a [babashka](https://babashka.org/) script at `./scripts/td`.
    1.  Babashka uses the [SCI](https://github.com/babashka/sci) Clojure
        dialect.
    2.  It adds helpful tooling for scripting.
    3.  A self contained script file can be created with the
        `#!/usr/bin/env bb` header.
    4.  The local installation is at `/opt/homebrew/bin/bb`.
        1.  Use `--help` to learn more.
        2.  Expressions can be tried out using `--eval`.
2.  The `td` script must implement `--help`.
    1.  At the top level.
    2.  For each command.
    3.  For each subcommand.
3.  For now it should implement `td message new`.
    1.  This creates a new message inside an existing task.
    2.  The message must conform to the spec in `tasks/taskdown.md`.

---
author: user
cargo:
  status: draft
  type: task
created: Sun 27 Sep 2006 - 15h 11m 00s
subject: TaskDown CLI
---

# TaskDown CLI

1.  Taskdown spec is in in `tasks/taskdown.md`.
2.  Create a [babashka] script at `./scripts/td`.
    1.  Babashka uses the [SCI] Clojure dialect.
    2.  It adds helpful tooling for scripting.
    3.  A self contained script file can be created with the `#!/usr/bin/env bb`
        header.
    4.  The local installation is at `/opt/homebrew/bin/bb`.
        1.  Use `--help` to learn more.
        2.  Expressions can be tried out using `--eval`.
3.  The `td` script must implement `--help`.
    1.  At the top level.
    2.  For each command.
    3.  For each subcommand.
4.  Implement `td message validate [id]`.
    1.  This checks that the message conforms to the spec.
5.  Then implement `td message new`.
    1.  This creates a new message inside an existing task.
    2.  This sub command should allow users to express all values allowed by the
        spec using command line flags.
    3.  It should validate the use of flags, and values provided to those flags,
        that would produce invalid messages.

  [babashka]: https://babashka.org/
  [SCI]: https://github.com/babashka/sci

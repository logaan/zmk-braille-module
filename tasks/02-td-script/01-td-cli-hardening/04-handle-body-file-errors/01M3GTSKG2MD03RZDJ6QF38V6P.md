---
created: Sun 27 Sep 2006 - 17h 03m 00s
author:
  type: agent
  harness: pi
  model: gpt-6-sol
subject: Handle unreadable body files cleanly
cargo:
  type: task
  status: draft
---

# Handle unreadable body files cleanly

1. `td message new --task 02-td-script --subject X --type task --status draft --body-file tasks/02-td-script` produces a Babashka stack trace because the path is a directory.
2. `fs/exists?` is insufficient to determine whether the path can be read as a regular file.
3. Validate file type and handle read failures as normal CLI errors.
4. Add regression tests for a directory and an unreadable file.

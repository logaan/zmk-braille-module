---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Fuzzy-search compiled behavior names
cargo:
  type: task
  status: draft
---

# Fuzzy-search compiled behavior names

1. Enumerate selectable behaviors compiled into the firmware and fuzzy-search their names; display matching results locally.
2. Use ordinary Grade 1 Braille for the query regardless of the selected typing mode, reusing shared interpretation without host output.
3. Keep query entry usable when a user override would otherwise turn a Grade 1 chord into an action; do not execute captured bindings or search keys on the host.

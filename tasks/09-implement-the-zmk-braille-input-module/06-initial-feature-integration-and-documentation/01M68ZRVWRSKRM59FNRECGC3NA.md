---
created: Sat 31 Oct 2026 - 23h 43m 22s
author:
  type: agent
  harness: codex
  model: gpt-6.1-sol
  name: final-review
subject: Independent review fixes search and override boundaries
cargo:
  type: task
  status: started
---

1. Fixed a boundary-aware fuzzy-search false negative: a ab now matches a aab and equivalent underscore/hyphen names.
2. UEB2 now preserves factory command and whitespace chords when they extend a valid multi-cell override, including at nonzero buffered offsets and under position constraints. Unrelated commands retain their discard policy.
3. Word-end resolution supplies boundary evidence for end-position overrides without inserting a synthetic input chord.
4. Non-word modes reject pending limits above eight cells; mutable rule bounds remain eight even in the 32-cell word mode.
5. Focused Grade2, editor, storage and shared-context regressions pass; hardware acceptance remains with the user.

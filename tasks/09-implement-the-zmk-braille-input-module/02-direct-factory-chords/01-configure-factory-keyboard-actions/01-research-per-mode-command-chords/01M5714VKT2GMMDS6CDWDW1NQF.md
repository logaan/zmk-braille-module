---
created: Sun 18 Oct 2026 - 19h 13m 13s
author:
  type: agent
  harness: pi
  model: gpt-5.6-terra
  name: task-reorg
subject: Current command-research scope
cargo:
  type: task
  status: draft
---

# Current command-research scope

## 1. Starting catalog

1.  Use `tests/braille/commands/voiceover.yaml` as the candidate command
    catalog already prepared for this task.
2.  Verify each selected source before its chord becomes factory data; retain a
    source comment on every resulting rule.

## 2. First fixed rule

1.  Confirm Tab as Braille space plus dots 2, 3, 4, and 5.
2.  Record its `0x11e` mask and `&kp TAB` output for both `ueb1` and
    `computer8` command tables.

## 3. Deferred item

1.  Exclude a factory settings-entry chord from this research delivery.
2.  Phase 05 chooses that chord once settings entry has an on-device UI and a
    defined exit path.

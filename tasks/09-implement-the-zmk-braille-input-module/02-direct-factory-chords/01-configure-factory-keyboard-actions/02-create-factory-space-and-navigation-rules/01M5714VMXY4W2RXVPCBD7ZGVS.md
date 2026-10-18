---
created: Sun 18 Oct 2026 - 19h 13m 13s
author:
  type: agent
  harness: pi
  model: gpt-5.6-terra
  name: task-reorg
subject: Direct-rule acceptance update
cargo:
  type: task
  status: draft
---

# Direct-rule acceptance update

## 1. Tab requirement

1.  Replace the fixture Tab rule with Braille space plus dots 2, 3, 4, and 5.
2.  Assert mask `0x11e` dispatches exactly `&kp TAB` in every initial command
    table.

## 2. Table requirements

1.  Replace the remaining fixtures only with researched direct factory rules.
2.  Keep literal Braille space and the selected whitespace metadata aligned with
    the real space, Return, and Tab rules.
3.  Validate the real command table against the current character table before
    selection.

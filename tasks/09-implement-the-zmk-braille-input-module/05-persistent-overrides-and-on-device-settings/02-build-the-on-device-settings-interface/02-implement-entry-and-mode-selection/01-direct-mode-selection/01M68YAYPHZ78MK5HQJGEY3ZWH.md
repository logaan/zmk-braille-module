---
created: Sat 31 Oct 2026 - 23h 18m 17s
author:
  type: agent
  harness: codex
  model: gpt-6.1-sol
subject: Runtime selection delivered with Computer8 coverage
cargo:
  type: task
  status: delivered
---

1. Added central bselect behavior with stable UEB1, Computer8, UEB2 and next-mode arguments. Only compiled modes cycle; unavailable modes preserve state. Successful selection discards pending input and live capture.
2. New selector harness passed eight combinations of optional Grade2, split and metadata; nine Computer8 tests passed, including every mapping and real keycodes; eight pinned devicetree tests passed.
3. README documents selection bindings. No character-table or physical chord changes; cheatsheet data unchanged. Hardware acceptance and mode persistence remain outstanding.

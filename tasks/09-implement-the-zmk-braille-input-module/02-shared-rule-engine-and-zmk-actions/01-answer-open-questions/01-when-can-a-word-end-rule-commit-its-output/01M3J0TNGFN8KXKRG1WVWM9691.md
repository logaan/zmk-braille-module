---
created: Mon 28 Sep 2026 - 04h 07m 54s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: 'Blocked: commands vs pending word-end input needs a decision'
cargo:
  type: task
  status: blocked
---

# Needs a user decision

1. Research so far:
    1. The initial modes' factory tables need no `word-end` rules: every `ueb1` reading is decided by earlier input (`references/research/ueb-indicator-context.yaml`), and `computer8` is single-cell.
    2. `word-end` rules therefore arise in the initial modes only from user overrides and engine fixtures.
    3. UEB bounds a word by a space, hyphen or dash (2.6.1); a following Braille space or hyphen chord can satisfy `word-end`, any other Braille chord rules it out and falls back to the shorter match, and with no following chord the candidate stays pending.
2. The plan does not settle what happens when a `word-end` candidate, or any complete match still waiting to be extended, is pending and the next event is a command chord (Return, Tab, arrows), a non-Braille key, or a layer change:
    1. Root 4.5.4 and "Track boundaries and pending output" item 2 discard pending input without host output.
    2. "Match longest valid rule with override precedence" item 3 commits the longest complete match when the next chord rules pending input out, then processes that chord.
    3. Phase 07's "Bound buffering and host-output latency" has Return and Tab resolve and send the buffered `ueb2` word before their own key.
3. Choice needed: for such events, should the engine (a) discard the pending input, (b) commit its non-`word-end` fallback and then act, or (c) treat Return and Tab as word-end evidence and commit the `word-end` output before the command?
4. In `ueb1` only output-free indicators can be complete-and-pending, so the choice affects user overrides, `computer8` multi-chord overrides, and `ueb2`.

---
created: Mon 28 Sep 2026 - 03h 01m 39s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: 'Decided: the next chord decides; no timer'
cargo:
  type: task
  status: delivered
---

# Decision: the next chord decides

## 1. Decision

1. There is no inter-chord timer; with release-terminated chords, the engine has no timers at all.
2. Ambiguous input stays pending until the next chord extends it or rules it out.
    1. Ambiguous means a complete match that a longer rule could extend, or an incomplete prefix such as dots 4 awaiting `@`.
3. When the next chord rules it out:
    1. Commit the longest complete match within the pending cells.
    2. Reprocess the remaining cells, then the new chord, exactly once each.
4. An incomplete prefix with no complete match is dropped with a display notice.
5. The display shows pending cells until they commit or drop.
6. Settings entry discards pending input (phase 01); other transitions are question 09.

## 2. Rationale

1. Braille learners and slow typists are target users; a timeout would commit or drop their input mid-thought.

## 3. Impact

1. `03-track-boundaries-and-pending-output` implements this policy.
2. Phase 05 shows pending cells.
3. Grade 2 latency questions start from "no timer".

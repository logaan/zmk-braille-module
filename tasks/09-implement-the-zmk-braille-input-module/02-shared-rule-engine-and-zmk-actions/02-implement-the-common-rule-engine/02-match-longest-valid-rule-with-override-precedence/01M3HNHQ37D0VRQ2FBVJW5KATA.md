---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Match longest valid rule with override precedence
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Match longest valid rule with override precedence

1. Create `braille_engine.c`; match the selected character table, that mode's factory command table (navigation and noncharacter functions), and the user override layer through the same nine-bit input path.
2. Prefer the longest valid sequence; for the same input, prefer the applicable user override over the factory rule.
3. Resolve ambiguity when the next chord extends or rules out pending input, with no timer: commit the longest complete match, reprocess the remaining cells exactly once, and drop an orphan incomplete prefix with a display notice.
4. Reject conflicting factory mappings within a mode, including between its character and command tables; intentional shorter/longer prefixes and distinguishable context rules are not automatically collisions.
5. Treat space-only, dots-only, and space-plus-dot identically; do not branch on individual characters or commands.
6. Unit-test matching alongside the implementation with fixture tables and in-memory overrides: longest valid sequence, shorter fallback with remaining input processed once, override precedence for identical inputs, a shorter override not defeating a longer valid sequence, and rejection of real factory collisions.

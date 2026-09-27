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
3. Resolve ambiguity when the next chord extends or rules out pending input, with no timer: commit the longest complete match, reprocess the remaining cells exactly once, and drop an orphan incomplete prefix with a display notice, then interpret the chord that ruled it out from scratch.
4. A rule is a candidate only when its `when` predicate holds in the context at the start of its sequence; pending cells do not change the context.
5. Reject conflicting factory mappings within a mode, including between its character and command tables; intentional shorter/longer prefixes and rules with the same cells but disjoint `when` predicates are not collisions.
6. Treat space-only, dots-only, and space-plus-dot identically; do not branch on individual characters or commands.
7. Unit-test matching alongside the implementation with fixture tables and in-memory overrides: longest valid sequence, shorter fallback with remaining input processed once, orphan prefixes dropped with the breaking chord interpreted from scratch, candidate filtering by `when`, override precedence for identical inputs, a shorter override not defeating a longer valid sequence, and rejection of real factory collisions.

---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Test Grade 1 against ICEB examples
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Test Grade 1 against ICEB examples

1. Build a rulebook-derived conformance suite beyond the per-mapping unit tests, covering cross-rule behavior: indicator interactions and termination, digit runs, multi-cell ambiguity, word boundaries, incomplete input, and uncontracted fallback.
2. Start from the fixtures in `references/research/ueb-literary-signs.yaml`, `ueb-numeric-mode.yaml`, `ueb-capitalisation.yaml`, and `ueb-grade-1-indicators.yaml`; each rulebook fixture records its section and page, and constructed fixtures are marked.
3. Record the authoritative rulebook section for each added fixture; treat the PDF as authoritative over text extraction.
4. Assert ordinary host-keystroke output, and check the real Grade 1 table against the `ueb1` command table for conflicting inputs.

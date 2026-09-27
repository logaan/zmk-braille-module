---
created: Mon 28 Sep 2026 - 00h 50m 49s
author: user
subject: Test ICEB contracted and fallback examples
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Test ICEB contracted and fallback examples

1. Build a rulebook-derived conformance suite beyond the per-rule unit tests, covering permitted and forbidden contractions, capital and numeric interactions, word-part eligibility, and uncontracted output where no rule applies.
2. Include every fixture in `references/research/ueb-grade2-contractions.yaml`, `references/research/ueb-grade2-word-boundaries.yaml`, and `references/research/ueb-grade2-indicator-interactions.yaml`; each is a rulebook example given as cells and expected print.
3. Check that the cases in `grade_1_text_reads_differently` give their `ueb1` print in `ueb1` and their `ueb2` print in `ueb2`.
4. Record the authoritative rulebook sections for each fixture; treat the PDF as authoritative over text extraction.

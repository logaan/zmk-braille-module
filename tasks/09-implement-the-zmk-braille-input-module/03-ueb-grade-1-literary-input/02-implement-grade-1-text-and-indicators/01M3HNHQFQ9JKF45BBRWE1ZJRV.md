---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Implement Grade 1 text and indicators
cargo:
  type: task
  status: draft
---

# Implement Grade 1 text and indicators

1. Produce host keystrokes, not Unicode Braille cells, using `braille_rules_grade1.c` and shared state.
2. Write each mapping's unit test alongside its implementation task; the separate conformance suite covers cross-rule behavior.

---
created: Mon 19 Oct 2026 - 23h 25m 06s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
subject: Computer8 context isolation verified
cargo:
  type: task
  status: delivered
---

1. Every real Computer8 character rule is immediate, any-context/any-position and neutral. `tests/host/test_computer8_modes.c` exercises all 94 cells in all 12 seeded numeric/caps/word-start combinations without context changes or factory buffering.
2. Literal capital/numeric/Grade-1-like cells and contractions stay ordinary Computer8 characters. Shared mapped/reserved and multi-cell overrides, all real commands, Braille space and Return/Tab whitespace paths remain available in both modes.
3. Tests verify mode changes silently discard a real UEB symbol prefix and pending overrides, reset seeded context, retain shared override data, and prevent cross-mode completion. Failed name selection preserves pending input and context.
4. Split/non-split, optional Grade-2 and pinned real ZMK keycode variants pass. Context is explicitly seeded through a test-only seam because phase-04 UEB transitions remain unimplemented; no production context API or new matcher branch was added.

---
created: Tue 20 Oct 2026 - 00h 30m 18s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
subject: Delivered contextual quotation signs, bar and backtick
cargo:
  type: task
  status: delivered
---

1. Added every required quotation variant, the contextual dots-236 question/opening-quote choice, vertical bar and standalone grave/backtick. Restored the full literary-sign research without changing the earlier 21 symbols.
2. Quotation signs emit straight US keyboard quotes; the distinct grave modifier and nonkeyboard signs remain unmapped. All literary cells use dots 1-6 only.
3. Unit tests verify each new sign, word-start preservation by opening brackets/quotes/apostrophe, mid-word questions, Grade-1-prefixed standalone questions and excluded grave modifiers. Earlier punctuation and all 21 symbols pass unchanged-output regressions.
4. Tests pass in split/non-split and optional-Grade-2 variants and with pinned real ZMK keycodes. No physical acceptance is claimed.

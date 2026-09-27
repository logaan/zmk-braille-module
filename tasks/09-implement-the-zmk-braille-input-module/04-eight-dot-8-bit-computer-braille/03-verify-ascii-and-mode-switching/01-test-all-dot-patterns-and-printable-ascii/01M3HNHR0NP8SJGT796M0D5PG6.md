---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Test all dot patterns and printable ASCII
cargo:
  type: task
  status: draft
---

# Test all dot patterns and printable ASCII

1. Assert every eight-bit dot pattern is explicitly mapped or reserved, including the zero-dot entry.
2. Verify all 95 printable ASCII characters: 94 non-space characters have one-cell input, and Braille-space-only emits literal space.
3. Include backtick, vertical bar, case, shifted symbols, and independence of dots 7 and 8; reserved input must follow the documented policy.
4. Check the real computer8 and Grade 1 tables against shared navigation/noncharacter data for conflicting factory inputs.

---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Test all dot patterns and printable ASCII
cargo:
  type: task
  status: draft
---

# Test all dot patterns and printable ASCII

1. Assert every 8-bit pattern is mapped or reserved, printable non-space US ASCII has one cell per character, and space follows the documented zero-dot/space-only policy.
2. Verify backtick and vertical bar are available in computer8.

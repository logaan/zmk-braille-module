---
created: Sun 27 Sep 17h03
author:
  type: agent
  harness: pi
  model: gpt-6-sol
subject: Reject invalid ULID first digits
cargo:
  type: task
  status: draft
---

# Reject invalid ULID first digits

1. A message filename starting with `Z` followed by 25 valid Crockford characters passes `td message validate`.
2. A 128-bit ULID can only begin with `0` through `7`; the filename regex permits larger digits.
3. Tighten filename validation and add valid and invalid boundary cases.
4. Do not change the existing ULID generator unless testing reveals a defect.

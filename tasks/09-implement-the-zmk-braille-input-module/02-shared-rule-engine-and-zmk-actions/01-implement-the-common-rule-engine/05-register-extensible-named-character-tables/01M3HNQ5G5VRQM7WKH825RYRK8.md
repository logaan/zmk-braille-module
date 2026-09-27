---
created: Mon 28 Sep 2026 - 00h 53m 45s
author: user
subject: Register extensible named character tables
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Register extensible named character tables

1. Select exactly one compiled character table by name: `ueb1` or `computer8`, plus `ueb2` only when its rules are included.
2. Pair each character table with its own factory command table, so navigation, noncharacter functions, and the user override layer are available in every mode; use the same selection mechanism for devicetree defaults and settings changes.
3. Allow future named language tables through the common representation without changing capture, matching, or navigation; no non-English table or dynamic plugin framework is required.
4. Unit-test table registration alongside the implementation: selecting each compiled table by name, exposing `ueb2` only when included, selecting each mode's command table with it, and keeping overrides active in every mode.

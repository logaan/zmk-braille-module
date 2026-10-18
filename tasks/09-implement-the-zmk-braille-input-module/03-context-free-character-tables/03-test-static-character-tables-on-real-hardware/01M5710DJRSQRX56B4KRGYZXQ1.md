---
created: Sun 18 Oct 2026 - 19h 10m 47s
author:
  type: agent
  harness: pi
  model: gpt-5.6-terra
subject: Test static character tables on real hardware
cargo:
  type: task
  status: draft
  worker: user
---

# Test static character tables on real hardware

## 1. Scope

1.  Exercise the phase-03 context-free UEB1 signs and the `computer8` table on
    the two baseline keyboards after their table and command-collision tests
    pass.
2.  Use a firmware configuration selecting each mode as needed; runtime mode
    persistence is not required for this acceptance.

## 2. Required checks

1.  In `ueb1`, type representative direct punctuation and multi-cell symbols,
    including a prefix followed by an unrelated chord.
2.  In `computer8`, type representative lowercase and uppercase letters,
    digits, shifted ASCII symbols, dots 7 and 8 behavior, and Braille space.
3.  Confirm the phase-02 Tab chord still produces Tab in both modes and does
    not collide with a character rule.
4.  Confirm selected reserved `computer8` patterns produce no host output;
    record any later display-feedback work separately.

## 3. Boundary

1.  This does not accept numeric/capital UEB context or settings and persistence
    flows.

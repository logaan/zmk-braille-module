---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Update installation and keymap documentation
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Update installation and keymap documentation

1. Replace hello-world claims and stale links in root `README.md`; document module installation through `zmk-config` without core changes and the enable option.
2. Provide a working parameterless binding/keymap example with arbitrary dot/space positions, the separate Braille key, and the distinct conventional space key.
3. Explain dot layout, 50 ms timeout default and configuration, `ueb1` default, compiled modes, short/long gestures, and conventional/Braille operation.
4. Document display/pending feedback, the full binding/search/parameter/confirmation flow, override precedence/capacity, persistent saves, removal, and reset.

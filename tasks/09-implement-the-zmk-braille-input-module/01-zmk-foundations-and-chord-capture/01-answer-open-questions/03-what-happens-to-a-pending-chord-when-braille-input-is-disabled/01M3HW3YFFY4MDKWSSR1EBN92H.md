---
created: Mon 28 Sep 2026 - 02h 45m 35s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: 'Decided: discard and swallow'
cargo:
  type: task
  status: delivered
---

# Decision: discard and swallow

## 1. Decision

1. Entering settings discards any unresolved multi-chord prefix without host output.
2. There is no Braille disable state (the module is always on), so no other disable transition exists.
3. A physical chord is never cut short by a layer change.
    1. ZMK routes each release to the binding that received the press, so Braille key releases stay with the driver and never leak to conventional bindings.
    2. The chord resolves normally on its last release, e.g. when a Toucan user lets go of the braille-mode-key a moment before the dots.
4. Pending multi-chord prefixes on layer changes, mode changes, and actions are phase 02's transition question; this decision is its starting point.

## 2. Impact

1. The settings-entry action and phase 02's transition policy implement the discard.
2. Chord-detector tests cover a layer change mid-chord.

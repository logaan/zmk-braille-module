---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Implement the braille-mode-key and settings action
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Implement the braille-mode-key and settings action

1. braille-mode-key(target-layer)
    1. Hold keeps the target layer active; pressing another key while holding it resolves as hold, so Braille keys on the target layer can be chorded while it is held.
    2. Tap switches to the target layer (to-layer); the user returns with a braille-mode-key on that layer pointing back, or any ordinary layer key.
    3. It is never a chord bit.
    4. Compose ZMK's hold-tap and layer behaviors if they meet these semantics; otherwise implement it in the module.
2. Settings-entry action
    1. A ZMK behavior action invoked by rules; phase 02 allocates its factory default chord, and users can rebind it through overrides.
    2. On entry, discard any unresolved multi-chord prefix without host output and suspend host typing; expose entry/exit hooks for phase 05.
3. There are no long-press gestures.
4. Unit-test alongside the implementation: holding while chording, tap to-layer and returning, exclusion from chord masks, and no host output across settings entry and exit, exercising the hooks rather than the later UI.

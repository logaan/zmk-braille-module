---
created: Mon 28 Sep 2026 - 02h 45m 55s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: Pin ZMK and the keyboard configs
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Pin ZMK and the keyboard configs

1. Find the most recent ZMK release and pin the module's development and CI builds to it.
2. Pin `../qbf-paintbrushes/basic-zmk/config/west.yml` to that release instead of `main`.
3. Move `../zmk-keyboard-toucan/config/west.yml` from `v0.3` to that release.
    1. Check that its community modules (cirque input module, rgbled widget, nice_view_gem) build against it.
    2. Record any blocker instead of forking or patching those modules.
4. Build both keyboards' existing firmware, without the Braille module, to show that the pinned baseline works before module work begins.
5. Commit each keyboard repository's changes in that repository, following its own contribution notes.

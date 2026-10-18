---
author:
  harness: claude-code
  model: claude-opus-5.5
  type: agent
cargo:
  status: draft
  type: task
  worker:
    harness: pi
    model: unknown
    type: agent
created: Mon 28 Sep 2026 - 02h 45m 55s
subject: Pin ZMK and the keyboard configs
---

# Pin ZMK and the keyboard configs

1.  Find the most recent ZMK release and pin the module's development and CI
    builds to it.
2.  Pin `../qbf-paintbrushes/basic-zmk/config/west.yml` to that release.
3.  Pin `../zmk-keyboard-toucan/config/west.yml` to that release.
    1.  Check that its community modules (cirque input module, rgbled widget,
        nice_view_gem) build against it.
    2.  Record any blocker instead of forking or patching those modules.
4.  Build both keyboards' existing firmware, without the Braille module, to show
    the pinned baseline works before module work begins.
5.  Commit each keyboard repository's changes in that repository, following its
    own contribution notes.

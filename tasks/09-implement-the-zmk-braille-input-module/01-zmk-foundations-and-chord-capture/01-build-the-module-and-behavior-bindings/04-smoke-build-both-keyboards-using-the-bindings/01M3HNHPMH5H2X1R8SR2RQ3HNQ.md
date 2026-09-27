---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Smoke-build both keyboards using the bindings
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Smoke-build both keyboards using the bindings

1. Add the module to the QBF Paintbrushes and Toucan configs on the pinned release, and check module discovery, the enable option, and the bindings in real west builds.
2. Use a Perkins-style home row on both: `f d s` = dots 1 2 3, `j k l` = dots 4 5 6, `a` = dot 7, `;` = dot 8, plus a Braille space key.
    1. QBF: on the base layer.
    2. Toucan: on a Braille layer reached with the braille-mode-key.
3. Check the default `ueb1` and a configured mode value at the build/configuration boundary.
4. Do not require character tables or settings screens; the capture tests verify runtime routing, and phases 03–06 verify actual modes.

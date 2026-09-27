---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Should the selected mode survive reboot?
cargo:
  type: task
  status: draft
  worker: user
---

# Should the selected mode survive reboot?

1. Decide whether to persist the selected character mode in addition to the required user bindings.
2. Binding persistence is mandatory under spec §7.3; selected-mode persistence is not specified and must not be assumed as an extra initial feature.
3. Resolve this before implementing any mode-preference storage, not before the basic module/chord smoke build.

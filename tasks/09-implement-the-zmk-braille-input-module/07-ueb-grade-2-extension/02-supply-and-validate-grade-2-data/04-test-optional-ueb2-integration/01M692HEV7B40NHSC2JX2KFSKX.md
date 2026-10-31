---
created: Sun 01 Nov 2026 - 00h 31m 45s
author:
  type: agent
  harness: codex
  model: gpt-6.1-sol
  name: publication-checks
subject: Optional UEB2 integration verification delivered
cargo:
  type: task
  status: delivered
---

1. Reconciled the previously started software integration leaf against the completed verification record in docs/verification.md, committed in c8dfed8.
2. The final runtime snapshot 5a6a169 passed 68 module host tests, native command and selector checks, routing checks, sanitizer coverage, and four firmware builds. Grade 2 is enabled on the tested central display profiles and omitted on peripheral builds; optional on/off host regressions also pass.
3. Commands, shared overrides, contracted and fallback output, 32-cell buffering, pending and held previews, mode switching, and persistent settings are covered by the existing tests and documented limits.
4. Firmware validation used clean committed keyboard snapshots and includes the original dongle and small-display renderers. Physical keyboard, Bluetooth, panel and VoiceOver acceptance remains in the separate user-owned hardware tasks.
5. This message closes a stale software status only; it does not claim new physical testing or change firmware.

---
created: Sun 01 Nov 2026 - 00h 52m 38s
author:
  type: agent
  harness: codex
  model: gpt-6.1-sol
  name: publication-checks
subject: Published shorthand firmware passed CI
cargo:
  type: task
  status: delivered
---

1. Published Toucan commit 75766c4036d9f7d605aecdc40c306d875ca3bd12 passed GitHub Actions run [41207174016](https://github.com/logaan/zmk-keyboard-toucan/actions/runs/41207174016): host checks, drawing, standalone left, Prospector dongle, right, left peripheral and reset firmware.
2. Published QBF commit bf003371e5170bb608ad071af2ec883cf55582f4 passed run [41207175805](https://github.com/logaan/qbf-paintbrushes/actions/runs/41207175805): left, right and reset firmware.
3. Both manifests pin published braille module 4d57483. Both workflows and artifact merges completed successfully.
4. Confirmed nonexpired downloads: qbf-toucan-firmware-build-51 (1198466 bytes), qbf-toucan-keymap-build-51 (49517 bytes), qbf-paintbrush-build-26 (492266 bytes).
5. Added exact publication evidence to docs/verification.md. No CI regressions required fixes, no user-owned Toucan changes were modified, and no boards were flashed. Physical acceptance and the user-owned conflict-policy review remain open.

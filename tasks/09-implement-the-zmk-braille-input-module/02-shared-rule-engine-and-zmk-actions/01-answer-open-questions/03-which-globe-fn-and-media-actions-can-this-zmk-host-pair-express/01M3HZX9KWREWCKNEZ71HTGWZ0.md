---
created: Mon 28 Sep 2026 - 03h 51m 51s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: Ten media keys and sticky Globe; no Fn, Dictation, DND
cargo:
  type: task
  status: done
---

# Result

1. Summary
    1. Researched ZMK `v0.3.0` (`edf5c08`) on Zephyr `v3.5.0+zmk-fixes` for a macOS host.
    2. Supported: brightness down/up, Mission Control, Spotlight, previous, play/pause, next, mute, volume down/up, all as Consumer-page `&kp C_...` bindings.
    3. Globe: `&sk GLOBE` works for Globe shortcuts only; macOS does not treat it as Fn.
    4. Not expressible: the Fn modifier (Apple vendor usage), Dictation/Siri, Do Not Disturb; Touch ID and power are out of scope.
    5. Fn-layer results (forward delete, Home/End/Page Up/Page Down, F1–F12) are sendable directly.
    6. `ZMK_HID_CONSUMER_REPORT_USAGES_BASIC` would disable Globe, Spotlight, and Mission Control; FULL is the default.
2. Detail: `references/research/zmk-globe-fn-and-media-actions.yaml`.
3. Tasks updated
    1. `02-.../03-.../01-research-per-mode-command-chords`: the function list to research.
    2. `02-.../03-.../03-add-supported-noncharacter-factory-bindings`: bindings, exclusions, consumer-report setting, node-label references.
    3. `06-.../01-.../02-check-the-macbook-air-coverage-target`: host checks.
    4. `06-.../02-.../02-document-mappings-and-test-evidence`: documented limitations.

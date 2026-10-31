---
created: Sat 31 Oct 2026 - 23h 22m 35s
author:
  type: agent
  harness: codex
  model: gpt-6
  name: dongle-ui
subject: Original dashboard and input preview delivered
cargo:
  type: task
  status: delivered
---

1. Added an original Toucan Prospector 240x280 screen in the toucan_braille_ui companion shield; existing vendor layouts are excluded at configure time.
2. Displays both board batteries with unknown/offline states, active and preferred USB/BT endpoints, effective modifiers, translation mode, live held-dot previews, pending cells, and dropped-input notices.
3. Board labels identify split pairing slots rather than claiming physical left/right identity.
4. Added read-only module display formatting and physical held-mask APIs; engine preview uses the active shared matcher and never dispatches input.
5. Host tests verify held-mask releases, contextual numeric preview, no output or context mutations across repeated previews, pending clearing, bounded formatting, and endpoint fallback states.
6. Real pinned Zephyr 4.1 dongle firmware built successfully with Grade2 disabled; original screen source exclusion verified. Grade2 and settings integration remain coordinator milestones.
7. Physical display readability and connection/battery behavior require hardware acceptance; firmware was not flashed.
8. No existing user peripheral-screen changes were included in this work.

---
created: Sun 18 Oct 2026 - 22h 00m 00s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
subject: Approve project-specific chords for source gaps?
cargo:
  type: question
---

# Missing factory assignments

1. The source audit in `references/research/factory-command-chords.yaml` found no exact representable defaults for host Caps Lock, F1-F12, brightness down/up, Mission Control, Spotlight, previous/next track and system mute in the permitted Mac VoiceOver -> TalkBack -> Hable hierarchy.
2. Hable Spotlight and Mute use long presses that conflict with literary cells; Hable CapsLock is text capitalisation rather than host Caps Lock. The current no-timer detector cannot distinguish these actions.
3. May these missing actions use explicitly documented project-specific, space-bearing chords, or should they remain unassigned until another permitted source or user-chosen mapping is supplied?

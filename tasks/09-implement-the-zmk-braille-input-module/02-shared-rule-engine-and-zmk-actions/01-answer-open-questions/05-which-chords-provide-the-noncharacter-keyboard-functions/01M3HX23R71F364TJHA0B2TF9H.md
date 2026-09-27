---
created: Mon 28 Sep 2026 - 03h 02m 04s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: 'Policy: VoiceOver, then TalkBack, then Hable One'
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Policy: VoiceOver, then TalkBack, then Hable One

1. The user set the source precedence for factory command chords.
    1. macOS VoiceOver first.
    2. Android TalkBack where VoiceOver does not handle the function.
    3. Hable One where neither does.
2. Conventions are mode-specific: VoiceOver's eight-dot conventions inform `computer8`, and its six-dot conventions inform `ueb1`.
3. Dots 7 and 8 for Delete and Return are welcome; research confirms whether VoiceOver uses them alone or with space.
4. Every final chord carries a comment naming its source.
5. The chord table is now agent research under this policy; the worker changed from user to agent.

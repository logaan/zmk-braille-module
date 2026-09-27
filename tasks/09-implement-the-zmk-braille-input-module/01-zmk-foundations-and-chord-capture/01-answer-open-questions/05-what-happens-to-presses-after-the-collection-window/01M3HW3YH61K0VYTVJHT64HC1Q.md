---
created: Mon 28 Sep 2026 - 02h 45m 35s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: 'Decided: chords end only when every key is lifted'
cargo:
  type: task
  status: delivered
---

# Decision: chords end only when every key is lifted

## 1. Decision

1. There is no collection window or timeout.
2. A chord starts at the first `&bc`/`&bspc` press.
3. Every Braille key pressed while any Braille key is still held joins the chord.
4. The chord resolves exactly once, when the last of its keys is released; the detector then returns to idle.
5. A late press therefore cannot occur, and rollover into the next chord is not supported.

## 2. Supersedes

1. The seed spec's collection window and the `timeout-ms` property (default 50 ms); both are removed.

## 3. Impact

1. `03-capture-chords-and-the-braille-mode-key/02-detect-and-finalize-nine-bit-chords` uses release-terminated chords.
2. The binding task drops `timeout-ms`.

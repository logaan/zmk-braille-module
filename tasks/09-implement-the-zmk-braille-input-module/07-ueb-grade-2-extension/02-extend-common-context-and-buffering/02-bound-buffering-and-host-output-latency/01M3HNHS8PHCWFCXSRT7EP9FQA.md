---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Bound buffering and host-output latency
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Bound buffering and host-output latency

1. Buffer each `ueb2` word, up to 32 cells, and resolve it only on whitespace.
    1. Whitespace is the Braille space chord; the word is sent, then the space.
    2. Return, Tab, and other command chords discard the buffered word under the common transition policy, like any other transition.
    3. Punctuation does not resolve a word; it stays in the buffer as part of the word.
2. When the buffer holds 32 cells, resolve it as if a word boundary were reached, send it, and start a new buffer with the next chord.
3. There is no other output-delay bound and no timer; other transitions discard the buffer under the common transition policy.
4. Raise the engine's per-mode pending bound to 32 cells for `ueb2`; keep the pending display synchronized with resolution and discards, scrolling to the most recent cells when the buffer is wider than the display.
5. Unit-test alongside the implementation: resolution on the Braille space; discard on Return, Tab, and other command chords; punctuation staying buffered; resolution at 32 cells with the next chord starting a new buffer; discard on other transitions; no lost or duplicated cells; and display synchronization.

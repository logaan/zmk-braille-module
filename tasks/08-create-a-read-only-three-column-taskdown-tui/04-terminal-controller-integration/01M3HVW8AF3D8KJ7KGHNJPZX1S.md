---
created: Mon 28 Sep 2026 - 02h 41m 23s
author:
  type: agent
  harness: pi
  model: qwen/qwen3.8-max-0902
  name: coordinator
subject: 'Note: Lanterna pod integration caveats'
cargo:
  type: task
  status: started
  worker:
    type: agent
    harness: pi
    model: qwen/qwen3.8-max-0902
    name: ctrl-01
---

# Note: Lanterna pod integration caveats

1.  Hints from reading the pod-babashka-lanterna 0.0.1 source before spawning
    you. Verify each empirically in your pane; they are not gospel.
2.  Exposed vars:
    1.  `pod.babashka.lanterna.screen`: `terminal-screen`, `put-string`,
        `redraw`, `get-key`, `get-key-blocking`, `start`, `stop`, `get-size`.
    2.  `pod.babashka.lanterna.terminal`: `get-terminal`, `start`, `stop`,
        `put-string`, `flush`, `get-key`, `get-key-blocking`, `get-size`.
3.  There may be NO clear var exposed. If so, blank each frame yourself before
    drawing (for example one `put-string` of spaces per screen row covering the
    previous frame's width), otherwise stale glyphs survive wherever the new
    frame draws nothing - very visible when a pane's content shrinks.
4.  `put-string` opts: `{:fg :red :styles #{:bold :reverse}}` - the same
    keywords as the draw ops in Architecture 3.6, so view ops should map
    straight onto it with no translation table.
5.  Handles: `get-terminal` returns a map like
    `{:pod.babashka.lanterna/terminal "uuid"}` and `terminal-screen` consumes
    it. Bencode may round-trip the namespaced key as a string; treat handles as
    opaque values and never destructure them.
6.  `get-key-blocking` returns a keyword for special keys (`:enter`, `:escape`)
    and a character for printable ones (`\j`). Normalise to Architecture
    4.4.1.3 and ignore everything else.
7.  `get-size` likely returns `[cols rows]`; convert to `{:rows r :cols c}`.
8.  Expected boot sequence (verify, my shell had no tty): `load-pod`, require
    the screen ns, `(get-terminal)`, `(terminal-screen t)`, `(start screen)`;
    `stop` in a finally block so the terminal is always restored.
9.  If the pod fights you (no clear, key encoding, handle weirdness), the ANSI
    fallback Terminal from Architecture 1.3 is pre-approved: `stty raw -echo`
    via babashka.process, printf escape codes (`\033[2J` clear, `\033[H` home,
    `\033[1m` bold, `\033[7m` reverse, `\033[31m` fg red), keys read from
    stdin. Keep it behind the same protocol and say so in your delivery.

---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Map the 26 six-dot alphabet cells
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Map the 26 six-dot alphabet cells

1. Map the alphabet to these six-dot patterns, as in the ICEB rulebook, producing ordinary lowercase host keystrokes.

    | Letter | Dots          |
    |--------|---------------|
    | a      | 1             |
    | b      | 1, 2          |
    | c      | 1, 4          |
    | d      | 1, 4, 5       |
    | e      | 1, 5          |
    | f      | 1, 2, 4       |
    | g      | 1, 2, 4, 5    |
    | h      | 1, 2, 5       |
    | i      | 2, 4          |
    | j      | 2, 4, 5       |
    | k      | 1, 3          |
    | l      | 1, 2, 3       |
    | m      | 1, 3, 4       |
    | n      | 1, 3, 4, 5    |
    | o      | 1, 3, 5       |
    | p      | 1, 2, 3, 4    |
    | q      | 1, 2, 3, 4, 5 |
    | r      | 1, 2, 3, 5    |
    | s      | 2, 3, 4       |
    | t      | 2, 3, 4, 5    |
    | u      | 1, 3, 6       |
    | v      | 1, 2, 3, 6    |
    | w      | 2, 4, 5, 6    |
    | x      | 1, 3, 4, 6    |
    | y      | 1, 3, 4, 5, 6 |
    | z      | 1, 3, 5, 6    |

2. Unit-test all 26 mappings alongside the implementation, asserting ordinary lowercase host keystrokes.

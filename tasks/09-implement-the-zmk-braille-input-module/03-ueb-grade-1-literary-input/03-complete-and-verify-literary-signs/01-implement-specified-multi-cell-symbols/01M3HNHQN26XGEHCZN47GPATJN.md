---
created: Mon 28 Sep 2026 - 00h 50m 47s
author: user
subject: Implement specified multi-cell symbols
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Implement specified multi-cell symbols

1. Map these multi-cell signs, with cells in order ("then" separates successive cells):

    | Key or sign              | UEB dots           |
    |--------------------------|--------------------|
    | `&` ampersand            | 4 then 1,2,3,4,6   |
    | `*` asterisk             | 5 then 3,5         |
    | `@` at sign              | 4 then 1           |
    | `^` caret                | 4 then 2,6         |
    | `#` number sign          | 4,5,6 then 1,4,5,6 |
    | `$` dollar sign          | 4 then 2,3,4       |
    | `%` percent sign         | 4,6 then 3,5,6     |
    | `~` tilde                | 4 then 3,5         |
    | `/` forward slash        | 4,5,6 then 3,4     |
    | `_` underscore           | 4,6 then 3,6       |
    | `+` plus                 | 5 then 2,3,5       |
    | `=` equals               | 5 then 2,3,5,6     |
    | `<` less-than            | 4 then 1,2,6       |
    | `>` greater-than         | 4 then 3,4,5       |
    | `(` open parenthesis     | 5 then 1,2,6       |
    | `)` close parenthesis    | 5 then 3,4,5       |
    | `[` open square bracket  | 4,6 then 1,2,6     |
    | `]` close square bracket | 4,6 then 3,4,5     |
    | `{` open curly brace     | 4,5,6 then 1,2,6   |
    | `}` close curly brace    | 4,5,6 then 3,4,5   |
    | `\` backslash            | 4,5,6 then 1,6     |

2. Unit-test every sign alongside the implementation, asserting ordered cells and correct host output, and each first cell followed by a chord that cannot continue it.

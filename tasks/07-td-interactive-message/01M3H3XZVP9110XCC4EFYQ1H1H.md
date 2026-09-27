---
created: Sun 27 Sep 2006 - 20h 10m 00s
author: user
subject: Merge script wizard into td message new
cargo:
  type: task
  status: started
---

# Merge script wizard into td message new

1. Show the script-wizard UI when -i/--interactive is passed to td message new.
2. Pre-populate fields given as CLI flags; keep them editable.
3. Ask whether to enter a body: default yes for task drafts, cancellations, questions, and answers; no otherwise.
4. Default the save confirmation to yes.

---
created: Sun 27 Sep 2026 - 22h 45m 00s
author: user
subject: Body prompt skipped for delivered -> done
cargo:
  type: task
  status: started
---

# Body prompt skipped for delivered -> done

1. Interactive message creation went straight to the editor when changing status from delivered to done, without asking whether to enter a body.
2. Always ask whether to enter a body before opening the editor.
3. Default to yes for draft and cancelled task messages, questions, and answers; default to no for other task statuses.

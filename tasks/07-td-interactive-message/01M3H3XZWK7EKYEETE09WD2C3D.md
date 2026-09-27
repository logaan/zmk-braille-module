---
created: Sun 27 Sep 2006 - 20h 25m 00s
author: user
subject: Interactive wizard merged into td
cargo:
  type: task
  status: delivered
---

# Results

1. `td message new -i` loads the script-wizard pod, prefills task/subject/type/status/author/worker/regarding/created/body from CLI flags or --file, and lets the user change them.
2. Body prompt defaults to yes for task drafts and cancellations, questions, and answers; no for other task statuses. Save confirmation defaults to yes.
3. The full message is edited in $EDITOR, validated on save, and drafts are kept on editor or validation failure; non-TTY stdin fails with a clear error.
4. Removed scripts/script-wizard.bb; expanded scripts/test-message-new-script-wizard to ten PTY modes and added non-TTY checks to scripts/test-td.

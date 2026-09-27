---
author: user
cargo:
  status: ready
  type: task
created: Sun 27 Sep 2006 - 19h 03m 00s
subject: Accepted with some modification
---

# Accepted with some modification

1.  Rather than using a regex for the date lets use a proper parsed date format.
2.  Update the system's official date format
    1.  Add in the year.
    2.  Add in the seconds.
    3.  Add a postfix sigil for minutes and seconds.
    4.  So we end up with "Sun 27 Sep 2006 - 16h 03m 32s"
3.  Backfill existing tasks in this repository. All with year `2006` and seconds
    of `00`.

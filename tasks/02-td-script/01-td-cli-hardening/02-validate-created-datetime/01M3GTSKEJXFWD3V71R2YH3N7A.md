---
created: Sun 27 Sep 2006 - 17h 03m 00s
author:
  type: agent
  harness: pi
  model: gpt-6-sol
subject: Validate created datetime ranges
cargo:
  type: task
  status: draft
---

# Validate created datetime ranges

1. `td message new --task 02-td-script --subject X --type task --status draft --created "Sun 99 Sep 99h99"` succeeds.
2. `td message validate` also accepts that value; `created-re` checks shape but not valid calendar day or time ranges.
3. Reject impossible dates and times while preserving the TaskDown datetime format.
4. Test both message creation and file validation. The format has no year, so weekday-to-date consistency may need a stated policy.

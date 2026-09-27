---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Select parameters and action bindings
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Select parameters and action bindings

1. Offer at least common key presses, modifier combinations, and Bluetooth profile commands among the compiled selectable behaviors.
2. Provide a second selection/entry step for required parameters and preserve their order; omit behaviors whose parameters cannot be edited safely.
3. Save ordinary common-model bindings; the limited editor must not restrict persistence or dispatch to a separate custom-action type.
4. Unit-test parameter selection alongside the implementation: required-parameter prompting, parameter order, the common key/modifier/Bluetooth editors, omission of behaviors without a workable editor, and saving ordinary common-model bindings.

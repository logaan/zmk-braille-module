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

1. Offer compiled selectable behaviors whose zero, one, or two parameters all have an editor below; omit the rest from selection.
    1. Classify each behavior's parameters with ZMK's `behavior_get_parameter_metadata()`; `references/research/zmk-behavior-enumeration.yaml` lists the metadata types and sources.
    2. A behavior with no parameter sets takes no parameters; `HID_USAGE` parameters use the keycode editor; the Bluetooth behavior's named commands and profile range use the Bluetooth editor.
    3. Omit behaviors that report no parameter metadata, and parameter kinds with no editor below, such as `LAYER_ID`.
2. Prompt for required parameters in order and preserve that order in the saved action.
3. Provide these parameter editors:
    1. Keycode: fuzzy-search ZMK keycode names, such as `LEFT_ARROW`, `TAB`, or `N4`, with the behavior-name search interface, then toggle Control, Shift, Option, and Command from a modifier checklist.
    2. Bluetooth profile commands: choose the command, then the profile number when the command takes one.
    3. Other behaviors whose parameters are keycodes, such as sticky keys, reuse the keycode editor.
4. Generate the keycode name list at build time from the pinned ZMK release's keycode headers.
5. Save ordinary common-model bindings; the limited editor must not restrict persistence or dispatch to a separate custom-action type.
6. Unit-test parameter selection alongside the implementation: required-parameter prompting, parameter order, keycode name search, modifier checklist encoding, Bluetooth profile commands, omission of behaviors without a workable editor, and saving ordinary common-model bindings.

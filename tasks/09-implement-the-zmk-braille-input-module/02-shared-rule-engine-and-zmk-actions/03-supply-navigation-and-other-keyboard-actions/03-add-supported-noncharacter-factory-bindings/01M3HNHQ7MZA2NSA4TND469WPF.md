---
created: Mon 28 Sep 2026 - 00h 50m 46s
author: user
subject: Add supported noncharacter factory bindings
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Add supported noncharacter factory bindings

1. Add per-mode rules for Tab, Return, Delete, Escape, arrows, Caps Lock, Shift, Control, Option, Command, and F1–F12, using the researched chords with their source comments.
2. Bind Shift, Control, Option, and Command as ZMK sticky-key actions (`&sk`).
3. Add the media/system and Globe rules with these bindings; `references/research/zmk-globe-fn-and-media-actions.yaml` holds sources and host notes.
    1. Display brightness down and up: `&kp C_BRI_DN`, `&kp C_BRI_UP`.
    2. Mission Control: `&kp C_AC_DESKTOP_SHOW_ALL_WINDOWS`.
    3. Spotlight: `&kp C_AC_SEARCH`.
    4. Previous track, play/pause, next track: `&kp C_PREV`, `&kp C_PP`, `&kp C_NEXT`.
    5. Mute, volume down, volume up: `&kp C_MUTE`, `&kp C_VOL_DN`, `&kp C_VOL_UP`.
    6. Globe: `&sk GLOBE`, a sticky modifier like Shift, Control, Option, and Command.
    7. Use Consumer-page `C_` keycodes, not the keyboard-page `K_` media keycodes.
4. Add no factory action for Dictation/Siri, Do Not Disturb, the Fn modifier, Touch ID, or power; they are documented limitations.
5. Keep ZMK's default `ZMK_HID_CONSUMER_REPORT_USAGES_FULL`; emit a build-time warning when `ZMK_HID_CONSUMER_REPORT_USAGES_BASIC` is set, because Globe, Spotlight, and Mission Control need usages above `0xFF`.
6. Reference factory behaviors through devicetree node labels, such as `DEVICE_DT_NAME(DT_NODELABEL(sk))`, and reference `&kp` and `&sk` from the module's devicetree, so ZMK's `/omit-if-no-ref/` cannot drop a behavior the factory tables use.
7. Unit-test the bindings alongside the data: each function in each initial mode, and sticky modifiers, including Globe, combining with the next Braille output.

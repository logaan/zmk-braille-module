# On-device Braille settings

Enable `CONFIG_ZMK_BRAILLE_STORAGE=y`, `CONFIG_ZMK_BRAILLE_SETTINGS_UI=y`, and a
renderer for `braille_editor_snapshot()`. The original Toucan dongle dashboard
supplies one. Firmware without the editor rejects `&bsettings` with `-ENOTSUP`
and keeps ordinary typing active. The storage/editor share the central input
work queue; a display thread must render a copied snapshot.

Enter with `&bsettings` or **space+dots 123456**. The latter is a project-local
shortcut, enabled only with the editor, not a claimed VoiceOver convention.
Settings input never invokes the actions being searched for or edited.

## Controls

| Input | Action |
|---|---|
| Space+1 / Space+4 | Previous / next item |
| Space+15 (also dot8 or space+8) | Activate selected item |
| Space+12 | Cancel current edit and return to main menu; leave from main menu |
| Space+145 (also dot7 or space+7) | Delete search character or incomplete indicator |
| Space+123456 | Leave settings without saving |

Previous/next, Return, Escape, and Delete positions follow the existing
[VoiceOver Mac braille command table](https://support.apple.com/en-us/103560).
Their menu meanings above are local adaptations. All controls are fixed and
ignore user overrides. When the screen says **Hold input chord**, every chord,
including these controls, is captured literally. Release one chord, then use
the following menu to add another, finish, or cancel. The eighth chord finishes
automatically.

Menu letter shortcuts use Grade 1 dot letters:

| Menu | Shortcuts |
|---|---|
| Main | `m` mode, `n` new binding, `u` unbind, `r` reset overrides, `d` done |
| Character mode | `c` Computer8, `u` UEB1, `t` UEB2 (two), when compiled |
| Captured input | `a` add chord, `f` finish, `c` cancel |
| Existing mapping | `o` override, `r` re-record, `c` cancel |
| Position | `a` any, `s` word start, `m` word middle, `e` word end |
| Modifiers | `c` Control, `s` Shift, `o` Option, `g` Command, `d` done |
| Actions | `f` finish actions, `a` add another, `c` cancel |
| Confirm save | `s` save, `c` cancel |
| Confirm removal/reset | `r` remove/reset, `c` cancel |

Compiled command/profile lists use previous/next and
activate. A character-mode selection applies immediately and persists through
the storage API. Modes unavailable in the build are absent.

## Creating a binding

1. Capture up to eight chords, reviewing each before adding another. A known
   factory or saved input offers override/re-record/cancel before editing.
2. Choose its word-position constraint. Overrides remain neutral with no
   numeric or capitals predicate.
3. Search compiled behavior display names or device names. Search uses Grade 1
   text regardless of the typing mode and ignores overrides. Space separates
   query words; underscores and hyphens in names are word boundaries. The best
   16 matches are shown by default (`CONFIG_ZMK_BRAILLE_SEARCH_RESULTS`).
4. Select required keycodes from the list generated from the build's ZMK
   `keys.h`. Toggle modifiers, then choose Done. Two keycode parameters are
   edited in order. Bluetooth command choices prompt for a profile only when
   their metadata requires one; display profile 1 is stored as index 0.
5. Add up to 16 ordered actions, then finish. Navigate the confirmation list
   to inspect **every** action's compiled device name and hexadecimal parameters.
   `s` saves; `c` leaves storage unchanged. Failed saves remain on confirmation
   with an error. Success exits to normal typing.

Only ready compiled devices with usable parameter metadata are selectable:
zero parameters, one/two HID-usage parameters, or named command values with an
optional bounded profile range. Layer parameters, missing metadata, the
Braille capture/layer/settings behaviors, and transparent are excluded.
Keeping a behavior compiled is a keymap/build concern; unreferenced behaviors
may be omitted by ZMK. Saved rules use the ordinary common rule/action schema,
not an editor-specific dispatch format.

Unbind reviews and removes a selected saved mapping. Reset overrides requires
confirmation and clears only module override records, including skipped ones;
it does not clear Bluetooth bonds or unrelated ZMK settings. Storage error and
skipped-record counts remain visible on the dongle dashboard. Storage byte and
flash capacity limits still apply even below the 256-rule identity limit.

## Validation

`python3 tests/host/test_editor.py` exercises literal control capture, eight
chords, 16 ordered actions and complete confirmation paging, cancellation,
failed saves and retry, deletion/reset, one/two keycode parameters, modifiers,
Bluetooth profiles, Grade 1 numeric query input, metadata filtering, generated
keycode entries, fuzzy alias/boundary matching, and absence of host output even
with conflicting user overrides. `test_build.py` checks headless settings
rejection and the enabled gate. Real Toucan dongle firmware also compiles with
UEB2, storage, and the editor enabled. Hardware interaction remains unverified.

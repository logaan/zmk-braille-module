---
created: Mon 28 Sep 2026 - 00h 50m 48s
author: user
subject: Fuzzy-search compiled behavior names
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Fuzzy-search compiled behavior names

1. Enumerate compiled behaviors at runtime; `references/research/zmk-behavior-enumeration.yaml` holds the ZMK sources and cost figures.
    1. Iterate ZMK's behavior registry with `STRUCT_SECTION_FOREACH(zmk_behavior_ref, item)`, skipping devices that are not `device_is_ready()`.
    2. The module's Kconfig selects `ZMK_BEHAVIOR_METADATA`, so each entry has a display name, such as "Key Press", and parameter metadata.
    3. Show and search the display name; also match the device name, such as `key_press`, which is the identifier actions store and invoke.
    4. Exclude the module's own Braille dot, Braille space, and braille-mode-key behaviors, and `transparent`.
    5. Selectability by parameter editor comes from the parameter-selection task.
2. Fuzzy-match with a case-insensitive subsequence matcher.
    1. `_`, `-`, and space in names are word boundaries the query need not type; a Braille space in the query starts a new word.
    2. Find each match with a forward then backward greedy scan, without allocation.
    3. Score matched characters, word-start and consecutive matches, whole-name prefix and exact matches, and penalize gaps; break ties by shorter name, then alphabetically.
    4. Keep the best K results in a fixed array, with K a Kconfig option defaulting to 16, and rescan every candidate on each query edit.
    5. With an empty query, list candidates alphabetically.
3. Show as many top results as fit on the central display, with the selected result marked; the next and previous controls move through the K results.
4. Use ordinary Grade 1 Braille for the query regardless of the selected typing mode, reusing shared interpretation without host output.
5. Edit the query and select a result with the settings controls.
6. Keep query entry usable when a user override would otherwise turn a Grade 1 chord into an action; do not execute captured bindings or search keys on the host.
7. Build the search over a generic source, a count plus the name or names at an index, so the keycode editor reuses the matcher, ranking, and result screen over keycode names.
8. Unit-test search alongside the implementation: enumeration of compiled behaviors with the exclusions, display-name and device-name matching, boundary-insensitive matching, fuzzy ranking and tie-breaks, the top-K bound, Grade 1 query input in either typing mode, query usability despite conflicting user overrides, and no host output from search keys.

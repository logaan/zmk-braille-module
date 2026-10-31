# Persistent modes and overrides

`CONFIG_ZMK_BRAILLE_STORAGE` defaults on when Zephyr settings are enabled. It
stores only keys below `braille/`; removing overrides never clears Bluetooth
bonds or other ZMK settings. The mode selector applies immediately and saves
its stable mode name. On boot an absent or unavailable mode uses the keyboard's
devicetree default. A write failure leaves the selected mode active for the
session and exposes an error instead of claiming that selection was saved.

The editor uses `braille_storage_save()` with an ordinary `braille_rule`:
1–8 nine-bit chords, a word-position constraint, and 0–16 ordered behavior
bindings. Overrides are neutral and have no numeric/capitals predicates. They
participate in the same longest-match algorithm as factory rules, in every
compiled character mode. Equal input prefers the override. A shorter override
does not defeat a longer valid match.

There are 256 persistent slots. Actions use a compact 16 KiB RAM pool rather
than reserving sixteen actions for every slot. All 256 single-action rules fit;
large multi-action rules share that byte budget. Flash capacity independently
limits persisted data. A full slot table, action pool, or settings partition
causes a visible save error; the previous live and saved mapping remain intact.
`CONFIG_ZMK_BRAILLE_STORAGE_POOL_SIZE` can change the RAM budget between 4 and
32 KiB. This is a capacity limit, not a promise that every possible set of 256
maximum-size records fits on every board.

Disk records are variable-length, versioned little-endian records: `BR`,
version, chord count, word position, action count, two bytes per chord, then
for each action a length-prefixed compiled device name and two uint32
parameters. Device pointers and local IDs never enter persistent storage.
Every action is resolved through `zmk_behavior_get_binding()` and checked by
`zmk_behavior_validate_binding()` on load. The runtime pool keeps the resolved
compiled name pointers only until reset.

Records load independently. Missing behaviors, unsupported formats, corrupt
records and invalid parameters skip that record while leaving its bytes stored.
Skipped records still reserve their slot, preventing unrelated saves from
silently overwriting them. A later firmware with the missing behavior can load
them again. Rebinding/removing a readable input identity replaces/deletes that
record. A completely corrupt identity can be removed by physical slot or by
resetting overrides. Reset deletes only override records; it keeps the mode.
A partial reset failure reports failure and retains all records that were not
yet successfully deleted.

`braille_storage_skipped_count()` and `braille_storage_last_error()` supply
boot/save notices. The optional notice hook does not emit host keystrokes.
The UI reads these from the serialized input/system queue. All storage edits
and snapshot enumeration use that same queue; they must not run concurrently
from an LVGL rendering thread.

Run `python3 tests/host/test_storage.py` for simulated power cycles, full
capacity, storage faults, skipped-record recovery, full-size serialization,
mode restoration and the shared matcher integration tests. Real firmware
builds additionally verify the Zephyr settings and behavior APIs. Hardware
power-cycle acceptance remains a separate physical check.

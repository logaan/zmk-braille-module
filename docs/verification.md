# Braille integration verification

Software evidence is separate from physical acceptance. These checks do not
flash any board or claim Bluetooth, panel, macOS, or VoiceOver acceptance.

## Verified software results

- All 82 host tests in committed snapshot `764ed49` passed, including the 11
  retired coordinator regressions. Full discovery passed 80 tests and skipped
  two optional PDF checks because the archive omitted the local rulebook;
  both checks then passed after linking that existing local reference. No
  coordinator exclusion is required. The new shorthand feature is verified
  separately from this publication baseline.
- Address/undefined-behavior sanitizers passed 38,400 deterministic random cells
  across UEB1, Computer8 and UEB2, with resets and previews after each input.
- Native ZMK command/character/HID/sticky-modifier tests passed, including the
  physical unavailable-mode selector regression and successful mode reset.
- Native key routing matched 81 recorded edges and 26 complete chords.
- Native mode/settings routing passed 12 checkpoints with no host key events.
- Actual LVGL renders passed visual inspection for held input, buffered UEB2,
  offline batteries, settings menu and full-action confirmation. The rotated
  QBF rendering and pagination tests also passed.

## Firmware builds

Final runtime sources were frozen at module `5a6a169`, clean Toucan checkout
`32c7d9d`, and clean QBF checkout `0befd92`. Later documentation-only commits
and manifest pin updates do not change those runtime source files. The Toucan
builds used archived committed checkouts, excluding pre-existing uncommitted
peripheral-display work. All images linked successfully and passed the
integrated configuration/keymap/source/link checks:

| Target | Flash bytes | RAM bytes | Result |
|---|---:|---:|---|
| Toucan Prospector dongle | 599,844 | 228,538 | UEB2, storage, editor, original dashboard |
| Toucan standalone left | 542,984 | 163,702 | UEB2, storage, editor, small-display widget |
| QBF left | 405,880 | 114,132 | UEB2, storage, editor, rotated OLED widget |
| QBF right | 245,152 | 50,476 | Peripheral; no braille engine |

Local build artifacts and SHA-256 hashes are retained in ignored
`build/integration/`; no firmware was flashed. The final module checker is
`tests/firmware/check_integrated.py`. The Toucan working checkout's 90 host
checks also passed; that test run includes its user's pre-existing display
changes, whereas the firmware builds above deliberately use committed source.

## Published firmware verification

After user-authorized pushes on 2026-11-01, GitHub built the published manifests
successfully, including targets beyond the four local builds above:

- [Toucan run 41205917402](https://github.com/logaan/zmk-keyboard-toucan/actions/runs/41205917402)
  verified commit `d937941`: host tests, keymap drawing, standalone central,
  shared right, peripheral left, Prospector dongle and settings-reset firmware.
  The workflow produced `qbf-toucan-firmware-build-50` and
  `qbf-toucan-keymap-build-50` artifacts.
- [QBF run 41205918241](https://github.com/logaan/qbf-paintbrushes/actions/runs/41205918241)
  verified commit `d0d013c`: left, right and settings-reset firmware. The
  workflow produced `qbf-paintbrush-build-25`.

Both manifests resolve module `c8dfed8` from its published repository. The
module repository has no separate GitHub Actions workflow. These are software
build results, not physical acceptance.

## Reproduce the software checks

Run the host suite from the module checkout with a C compiler, Python, the
pinned `build/baseline` ZMK/Zephyr headers, PyYAML and kconfiglib:

```sh
uv run --no-project --with pyyaml --with kconfiglib \
  python -m unittest discover -s tests/host
```

The retired `test_coordinator.py` remains a regression suite for the old
alphabet-worker script. Its stale mocks were repaired in `764ed49`; all 11 tests
now isolate terminal reaping, worker startup and interactive verification, and
reject any unmocked subprocess call. The script itself is unchanged. No suite
exclusion is needed.

This includes deterministic address/undefined-behavior sanitizer coverage of
38,400 random cells across all modes, boundary resets, and held-cell previews.
It supplements expected-output tests; random input alone does not establish
Braille linguistic correctness.

Native ZMK checks use `zmkfirmware/zmk-build-arm:3.5` and the pinned v0.3.0
workspace, and execute the real unmodified kernel/HID path:

```sh
./scripts/test-commands
./scripts/test-routing
./scripts/test-mode-settings
```

The mode/settings native fixture isolates the routing gate with a test-only
entry behavior. The public `&bsettings` rejects headless firmware; the real
editor has separate workflow, metadata, fuzzy-search and firmware checks.

Build QBF on its own release workspace:

```sh
./scripts/smoke-build qbf-left qbf-right qbf-computer8
```

Toucan uses ZMK commit `4dd95d7840eb3d5a3f59bb29fbf1ab054715ef85` / Zephyr 4.1
and the `zmkfirmware/zmk-build-arm:stable` image. Initialize a separate west
workspace from the Toucan manifest. From its root, with `TOUCAN` pointing to a
clean committed keyboard checkout:

```sh
west build -p always -s zmk/app -d build/toucan-dongle -b xiao_ble//zmk \
  -S studio-rpc-usb-uart -- -DZMK_CONFIG="$TOUCAN/config" \
  -DZMK_EXTRA_MODULES="$TOUCAN" \
  -DSHIELD="toucan_dongle prospector_adapter toucan_braille_ui"
python3 "$TOUCAN/scripts/check-braille-dashboard.py" build/toucan-dongle
```

The module is normally discovered from the manifest. For local unpushed module
verification, an additional `ZMK_EXTRA_MODULES` entry may point to an explicit
source snapshot; record its commit rather than claiming the remote pin was
fetched. The QBF smoke script verifies manifest discovery separately.

## Display evidence

The Toucan repository's `scripts/preview-braille-dashboard.py --lvgl PATH`
compiles the actual `screen.c` with LVGL and stubs only platform state
acquisition. Saved renders cover held UEB1 `13456` → `y`, UEB2 pending `1` plus
held `14` → provisional `according`, offline states, settings, and confirmation.
The offline render caught a battery-label wrapping collision; the final compact
B1/B2 labels fit both states.

`./scripts/preview-small-display` renders the actual small-screen layout with
LVGL 8 and checks canvas rotation. The QBF's 128×32 physical OLED is used as a
32×128 portrait canvas; the narrow renderer pages complete action details and
preserves complete cell labels when shortening long buffers.

## Resource budgets

Set `CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE=6144` explicitly for the tested UEB2
and editor display profiles. ARM GCC `-fstack-usage` reports showed a nested
UEB2 path of at least 2,176 bytes before ZMK event/behavior frames; the preview
path needs at least 2,400 bytes before editor snapshot and queue overhead.
The final editor-enabled preview chain uses about 2,976 bytes of reported
local frames before library/queue overhead, leaving about 3 KiB within the
6 KiB work queue. The former 2 KiB baseline was insufficient. Stack reports are a static budget
check, not a measured hardware high-water mark.

The persistent override pool is 16 KiB plus bounded slot metadata. Up to 256
small overrides fit in RAM; long 16-action rules may reach the pool or flash
limit earlier. `-ENOSPC` is visible, and a failed save preserves its previous
mapping. The display caches unchanged previews, invalidating on held cells,
pending input, context, mode, transient rule changes and persistent pool
revision.

## Physical acceptance still required

1. Flash the matching central/peripheral images for the chosen profile and
   verify split pairing. B1/B2 are pairing slots, not guaranteed left/right.
2. Check both battery percentages, loss/reconnection behavior, selected Bluetooth
   channel, USB/Bluetooth fallback, and effective modifier indicators.
3. Type all printable US characters and required command chords from the
   [shared coverage checklist](../references/key-coverage.md). Verify actual
   host output on a US layout in every mode and switch while a chord is held.
4. Verify UEB2 word buffering, 32-cell boundaries, capital/numeric/Grade 1
   indicators, overridden commands and no duplicated output.
5. Exercise settings capture, search, complete action confirmation, cancel,
   failed/capacity saves, removal and reset. Confirm mode and bindings survive
   actual power cycles, and unrelated Bluetooth settings remain intact.
6. Check panel readability, settings paging and long-buffer movement on both
   display sizes. Validate that previews track currently held dots and remain
   provisional where the next input can change the interpretation.
7. Check VoiceOver navigation and Globe shortcuts. Globe is a consumer usage,
   not true Fn; `ZMK_HID_CONSUMER_REPORT_USAGES_FULL=y` is required. No claims are
   made for Touch ID, power, other languages or nonvisual feedback.

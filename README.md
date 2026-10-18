# ZMK Braille module

An out-of-tree ZMK module for chorded Braille input. **Not usable Braille input
yet.** Central-local dot/space behaviors now route press/release edges by binding
parameter, on any layer, to the release-terminated detector. Capture now emits
one nine-bit mask on the last Braille-key release, without a timer. The downstream
completion consumer still returns `-ENOTSUP`; matching/output are not implemented.
The mode key now composes ZMK layer/hold-tap behaviors, and settings entry/exit
hooks isolate captured input from typing. There is no matching, character table,
settings UI, or Braille HID output yet.

## Pinned development baseline

[`west/west.yml`](west/west.yml) is the shared development/CI dependency manifest.
It pins ZMK **v0.3.0** (`edf5c0814fd3ea202e43aad2d68fd32e882a518c`),
verified as GitHub's latest non-prerelease release on 2026-10-18. Both baseline
keyboard manifests use the same release. Keep future module build/CI wiring on
this manifest rather than a floating ZMK branch.

The release uses Zephyr 3.5 and LVGL 8; the tested local build container is
`zmkfirmware/zmk-build-arm:3.5`. Baseline boards are `nice_nano_v2` for the QBF
Paintbrushes and `seeeduino_xiao_ble` for the Toucan. The manifest also freezes
the Toucan's upstream Cirque `toucan` snapshot and pins its RGB widget release;
its `nice_view_gem` display is supplied by the keyboard repository itself.

## Try it locally

In a pinned ZMK west workspace, include the module through the ordinary
`zmk-config` manifest, or pass the local checkout with `ZMK_EXTRA_MODULES`
(replace the paths and board with your own):

```sh
west build -s /path/to/zmk/app -b <board> -- \
  -DZMK_EXTRA_MODULES=/path/to/braille-module \
  -DZMK_CONFIG=/path/to/zmk-config
```

Add `CONFIG_ZMK_FEATURE_BRAILLE=y` to the central firmware's `.conf` file.
The option is off by default and unavailable on split peripherals. Pristine
build after changing it. Preserve any other extra modules in the semicolon-
separated `ZMK_EXTRA_MODULES` list (the Toucan also needs its keyboard module).
No ZMK-core changes or additional west dependencies are needed.

For manifest discovery, add this project to your config's existing `projects`
list and pin `revision` to the reviewed module commit:

```yaml
- name: zmk-braille-module
  url: https://github.com/logaan/zmk-braille-module
  revision: <reviewed-module-commit>
```

## Both-keyboard smoke builds

Both sibling manifests now pin the published module revision containing the
production bindings. QBF enables the module only on its left central and puts
Perkins Braille on BASE. Toucan keeps QWERTY/NAV/SYM/ADJ/TAP, adds BRL at index 5,
and replaces the duplicate left SYM thumb with `&bmode 5`; BRL's left thumb or
top-right key returns with `&to 0`. Its conventional Space stays conventional on
BASE and becomes `&bspc` only on BRL. Both use the schema's `ueb1` default.

Physical `f d s` = dots 1 2 3; `j k l` = 4 5 6; `a` = 7; `;` = 8.
QBF uses the key under `j` for Braille space and updates its hand-maintained OLED
legends to match. Toucan uses its ordinary Space position for Braille space.
**These are not working text input yet:** `bmode` can now enter BRL by tap or
hold, but completed chords have no production matcher/output consumer yet.

Prepare the shared workspace once, if it is not already present:

```sh
mkdir -p build/baseline/manifest
cp west/west.yml build/baseline/manifest/west.yml
docker run --rm -v "$PWD/build/baseline:/work" -w /work \
  zmkfirmware/zmk-build-arm:3.5 bash -ceuo pipefail '
    west init -l manifest
    west update --narrow --fetch-opt=--depth=1
    west zephyr-export
  '
```

With sibling checkouts at `../qbf-paintbrushes` and `../zmk-keyboard-toucan`:

```sh
./scripts/smoke-build
# Or select targets for a focused rerun:
./scripts/smoke-build qbf-left toucan-left toucan-computer8
# Recheck existing generated artifacts without rebuilding:
python3 tests/firmware/check_smoke.py --workspace build/baseline \
  qbf-left qbf-right toucan-left toucan-right toucan-computer8 qbf-reset toucan-reset
```

The script reuses the workspace and Docker 3.5, temporarily selects a generated
manifest that adds the keyboards' pinned module project, and mounts **this local
source checkout** at that project's west path. This intentionally substitutes
local work for the remote module pin during development. West discovers the
module normally (no Braille `ZMK_EXTRA_MODULES` injection); the Toucan keyboard
itself still uses `ZMK_EXTRA_MODULES=/toucan`. The prior workspace manifest
selection is restored on exit. Do not run concurrent west/config operations in
this workspace while smoke builds are running. Source dependencies are not
updated or patched; a new workspace should use the preparation command above.
`WORKSPACE`, `QBF` (the `basic-zmk` directory), and `TOUCAN` override paths.

Default targets are both halves, both settings-reset images, and a second
Toucan-central build with `tests/firmware/computer8.overlay`. That override checks
only the configured named-mode boundary; no `computer8` table/runtime is claimed.
Assertions inspect generated keymaps/default names, central-only feature/source
gating, all four linked behavior devices, retained displays/Studio/pointing,
and nonempty UF2s. Logs/exit codes are `build/baseline/logs/smoke-*`; artifacts
are `build/baseline/build/smoke-<target>/zephyr/zmk.uf2`. Existing sibling CI
workflows use their manifests to fetch the **published pinned** module instead
of the local substitution, including both halves and reset images. Publish a
new module commit before updating/pushing either dependent keyboard pin.
No hardware is flashed by these commands.

## Devicetree interface

Include the module's default nodes explicitly in your central keymap:

```dts
#include <behaviors/braille.dtsi>
```

| Binding | Cells | Compatible | Meaning |
| --- | --- | --- | --- |
| `&bc N` | 1 | `zmk,behavior-braille-dot` | Dot number 1–8, not a bitmask |
| `&bspc` | 0 | `zmk,behavior-braille-space` | Ninth chord bit; not conventional space |
| `&bmode LAYER` | 1 | `zmk,behavior-braille-mode` | Hold target layer while chording; tap switches to it |
| `&bsettings` | 0 | `zmk,behavior-braille-settings` | Settings-entry action, also usable by rule actions |

Dot/space callbacks validate parameters and forward a single-bit press/release
edge. Dots 1–8 use `1 << (N - 1)` (bits 0–7); `&bspc` uses `0x100` (bit 8).
The original ZMK event, including position, layer, timestamp, and split source,
is preserved. There is no on/off state, fixed key-position mapping, or layer
filter: only invoking a Braille binding enters this path. Conventional
`&kp SPACE`, letters, and layer keys retain their ordinary ZMK behavior.
ZMK retains the press-time layer state for release routing; the driver does
not look up the current layer again on release.

`braille_chord_key_event()` starts capture on the first press, accumulates every
Braille bit pressed while any Braille key remains held, and calls
`braille_chord_resolved(mask, last_release_event)` exactly once on the last release.
Early releases do not remove accumulated bits; late joins and re-presses stay in
the same chord. Space-only (`0x100`), dot-only, and space-plus-dot masks are all
ordinary chords. Held keys are identified by position and split source, not
layer; different physical keys bound to the same dot must both release.
Duplicate edges are ignored. State is idle before the synchronous completion
callback, even if it fails. Capture has no timer or timeout property.

**Output is not implemented yet:** the weak `braille_chord_resolved()` consumer
returns `-ENOTSUP` until the shared engine supplies it. That error propagates on
the final release; capture itself is implemented. The detector is bounded by
`ZMK_KEYMAP_LEN` simultaneous keys; overflow returns `-ENOSPC` without changing the chord. Invalid masks or a
changed bit for an already-held key return `-EINVAL`. Input callbacks must be
serialized, as on ZMK's central behavior path. The mode key selects a layer,
not a character table. Use an existing keymap layer number as its argument.

### Mode key and settings lifecycle

`&bmode LAYER` forwards its single target to both parameters of the internal
`braille_mode_hold_tap` device. It uses ZMK's `hold-preferred` hold-tap with `&mo`
for hold and `&to` for tap, a 200 ms tapping term, no quick-tap window, and
`hold-while-undecided`. Thus the target activates immediately; another key press
resolves hold before that key is dispatched, even within 200 ms. A timer-resolved
hold also stays momentary. Release ends the momentary layer; a tap instead
switches to the target until an ordinary layer key or another `&bmode` switches
back. Keep a return binding on the target layer. Neither mode edges nor the
underlying layer actions enter chord masks. The only timer is ZMK's mode-key
hold/tap decision; **chord capture still has no timer**. Runtime mode bindings
reject nonexistent layer IDs and nonzero unused parameters; metadata exposes a
layer parameter. Include `behaviors/braille.dtsi` for custom mode devices too.
The internal hold-tap node may be overridden to tune `tapping-term-ms`; retain
its flavor, no-quick-tap, bindings, and hold-while-undecided for these semantics.

`&bsettings` is a zero-parameter, central-local ordinary ZMK behavior. Its press
enters settings; release does not exit. Rule actions can invoke the same behavior
through the normal ZMK API. No factory entry chord is assigned here; command-chord
research/overrides own that mapping. `include/braille_settings.h` exposes:

- `braille_settings_enter(event)` / `braille_settings_exit(event)` and
  `braille_settings_active()`: idempotent serialized lifecycle, not a UI.
- `braille_input_reset()`: weak engine hook to discard unresolved multi-chord
  input and reset context **without host output**. The future shared engine must
  implement it; the current reserved engine has no pending state to discard.
- `braille_settings_changed(active, event)`: weak UI notification, called only
  after state changes and reset; the phase-05 UI calls exit explicitly.
- `braille_settings_chord(mask, event)`: settings-only capture consumer. While
  active, completed chords go here, never to `braille_chord_resolved`. Its weak
  default consumes without output; UI errors propagate without typing fallback.

Both boundaries discard an in-flight chord through the next idle point, retaining
physical key ownership and dropping late joins. Nothing is committed or replayed
on exit. These hooks/gates suspend **module-generated Braille typing**, not
conventional keymap behaviors or an already-held host key. The later ordered
host-action dispatcher must also check `braille_settings_active()` before output,
including queued actions; the capture gate is not a substitute for that check.
No settings screen, persistent storage, matcher, or output dispatcher is added.

The same include supplies the `braille` configuration node (`zmk,braille`).
Its `default-mode` string defaults to `"ueb1"`. A keyboard may override it:

```dts
&braille {
    default-mode = "computer8";
};
```

This stable name is not an enum: future named tables need no schema change.
It must name a compiled table. The later named-table/storage implementation
will read `DT_PROP(DT_NODELABEL(braille), default_mode)` when no selection is
saved or the saved mode is not compiled in. No tables, persisted selection,
or runtime fallback algorithm are implemented yet.

Zephyr 3.5 enforces the binding cell counts but cannot constrain individual
phandle-array argument values in YAML. With the feature enabled,
`src/braille_bindings.c` rejects dot arguments outside 1–8 at compile time in
enabled nodes' `bindings` arrays, including custom dot devices. This does not
validate dynamically constructed rule/Studio bindings. The input driver also
rejects invalid runtime dot values before shifting, and rejects nonzero unused
parameters (dot `param2`, or either space parameter), returning `-EINVAL`
without calling the detector. Dot 1–8 and zero-parameter space metadata are
provided when ZMK behavior metadata is enabled. Parameterized macros should
use a valid dot number as their replaceable placeholder, not zero.

## Source layout and checks

- `dts/bindings/` and `dts/behaviors/braille.dtsi`: production schemas, default
  devices, and named per-keyboard default-mode configuration.
- `src/behavior_braille.c`: central-local dot/space input routing and runtime
  validation, one-parameter mode-key adapter, and settings-entry behavior.
  No character dispatch or rule matching.
- `include/braille_chord.h`: single-bit input, completed-mask hook, and mask constants.
- `src/braille_chord.c`: bounded held-key tracking and timer-free nine-bit capture;
  the downstream completion consumer remains explicitly unsupported.
- `include/braille_settings.h` / `src/braille_settings.c`: lifecycle/reset/UI hooks
  and capture-to-consumer typing gate, without a settings UI.
- `src/braille_bindings.c`: build-time dot-argument assertions.
- `src/braille_engine.c`: reserved shared matcher/buffer translation unit.
- `include/braille_rules.h`: reserved common rule/action schema; only a forward
  declaration, not a temporary alternate output representation.
- `src/braille_rules_{grade1,nav,computer8}.c`: reserved rule-data translation units.
- `src/braille_rules_grade2.c`: optional placeholder selected by
  `CONFIG_ZMK_BRAILLE_GRADE2=y` (off by default). This does not implement UEB2.

Run the focused host checks with Python 3, CMake, and a C compiler:

```sh
python3 tests/host/test_build.py
python3 tests/host/test_routing.py
python3 tests/host/test_chord.py
python3 tests/host/test_settings.py
# Optional ASan/UBSan runs of routing, detector, and settings checks:
CC='cc -fsanitize=address,undefined -fno-omit-frame-pointer' python3 tests/host/test_routing.py
CC='cc -fsanitize=address,undefined -fno-omit-frame-pointer' python3 tests/host/test_chord.py
CC='cc -fsanitize=address,undefined -fno-omit-frame-pointer' python3 tests/host/test_settings.py
# With the pinned workspace present, also check the real Kconfig parser:
PYTHONPATH=build/baseline/zephyr/scripts/kconfig python3 tests/host/test_kconfig.py
# Production DT schemas + C assertions, using the same pinned workspace:
python3 -m venv build/bindings/venv
build/bindings/venv/bin/pip install PyYAML==6.0.3
build/bindings/venv/bin/python tests/host/test_devicetree.py
# Parse both actual sibling keymaps; check layouts, return paths, pins/legends:
build/bindings/venv/bin/python tests/host/test_keyboards.py
```

These check enabled/disabled source selection, optional Grade 2 isolation,
every combination of enabled behavior compatibles, central locality, and the
explicit unsupported downstream typing consumer, mode forwarding, and settings
behavior validation. Settings checks exercise pending-prefix discard, hook ordering,
idempotence, UI-only routing/error propagation, and chords spanning either or both
boundaries, with split off/on. Detector checks
compile the production detector with only a completion spy: all 511 nonempty
masks, overlap/early release/late joins/re-presses, consecutive chords, layer and
split-source identity, duplicate bindings/edges, invalid input, bounded capacity,
consumer failure, and reentry. Routing checks cover all eight dot
parameters and space across 32 layers and seven positions, multiple dot devices,
original event fields (including split source), invalid runtime parameters,
metadata, and downstream error propagation, with split/metadata both on and off.
The host shim is not a Zephyr runtime and does not prove hardware behavior.
The devicetree tests run Zephyr's actual parser/header generator, then compile production assertions against real Zephyr headers with
the host compiler (`CONFIG_ARCH_POSIX`); they test valid dots, rejected boundary
values, cell schemas, custom devices, extensible names, and the `ueb1` default.
Set `ZEPHYR_BASE` and `ZMK_APP` to use a different pinned workspace. They do not
run a Zephyr kernel. `tests/firmware/bindings.overlay` is a Paintbrush-central
compile/link fixture, not a usable keymap or a replacement for the actual
both-keyboard smoke builds above. The existing `tests/braille/*.yaml` files are
future conformance data, not an executable engine test suite yet.

Run the real pinned ZMK integration tests in Docker:

```sh
./scripts/test-mode-settings
# Recheck the mode/settings log without rebuilding:
python3 tests/firmware/check_mode_settings.py build/baseline/logs/mode-settings-native.log
```

The mode/settings fixture runs 12 assertion checkpoints through actual keymap,
ZMK hold-tap/layer behaviors, mock scan events, production capture and settings
gates. It checks interrupted/timed holds, chording while held, tap-to-layer and
both return paths, release ownership after mode release, mask exclusion, and
settings invoked as a fixture rule action. A fixture pending consumer/UI verifies
reset-before-notice, no stale prefix replay, and no host keycode events. It does
not implement the future matcher or settings UI. Logs are
`build/baseline/logs/mode-settings-{build,native}.log`; executable is
`build/baseline/build/mode-settings-native/zephyr/zmk.exe`.

The earlier routing regression remains unchanged:

```sh
./scripts/test-routing
# Recheck its log without rebuilding:
python3 tests/firmware/check_routing.py build/baseline/logs/routing-native.log
```

This builds/runs `native_posix_64` with the real production behavior driver,
keymap/layer APIs, mock scan driver, and Zephyr kernel. A separate **test-only**
module in `tests/firmware/routing/` observes edges through a linker wrapper that
always calls the production detector, supplies only the unavailable completion
consumer, and observes conventional keycode events without consuming them.
The reviewed 81-event routing snapshot checks every dot on two different layouts,
Braille space versus conventional Space/A/B (also while a dot is held),
transparent-layer fallback, and press-time release ownership when switching
layers with a dot, Braille space,
or conventional space held. A second snapshot checks 26 completed masks and their
last-release events, including overlapping keys, late joins, re-presses, two
physical keys sharing a dot, and a chord spanning a layer switch. Each completion
must immediately follow its final release. It also validates dot/space metadata
through real ZMK APIs. No ZMK source is patched or layer router reimplemented
in the test.

`WORKSPACE` overrides the shared workspace path. Build/run logs are
`build/baseline/logs/routing-{build,native}.log`; the native executable is
`build/baseline/build/routing-native/zephyr/zmk.exe`. This test does not connect
a host transport: the native board's expected `USB endpoint is not supported`
messages do not negate its keycode-event checks. It does not test a finished
rule engine, wireless split transport, physical hardware, or host typing.

See [the implementation task](tasks/09-implement-the-zmk-braille-input-module/)
for requirements and acceptance gates. The unchanged
[ICEB rulebook](references/README.md) is a separate reference under its own
noncommercial/no-derivatives license; the MIT license in `LICENSE` applies to
the module code, not the PDF.

## Tasks

This project tracks its work in `tasks/` using TaskDown. TaskDown's implementation,
tests, and its own development tasks now live in the separate Git repository at
`~/code/personal/babashka/taskdown`.

The existing entry points remain as small compatibility launchers:

```sh
./scripts/td system                  # Read the TaskDown reference
./scripts/td message new -i          # Work on this project's tasks
./scripts/task-down/task-down        # Browse this project's tasks
```

By default the launchers expect the TaskDown checkout at
`../../babashka/taskdown`, relative to this repository. Set `TASKDOWN_HOME` to use
another checkout. Neither launcher changes the current working directory, so
they continue to operate on this project's task data.

# ZMK Braille module

An out-of-tree ZMK module for chorded Braille input. **Currently this is build
scaffolding, not usable Braille input.** The behavior devices register on the
central half, but their press/release callbacks return `-ENOTSUP`. There is no
chord capture, layer switching, matching, character table, settings UI, or HID
output yet. The former startup-log stub has been removed.

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

These schemas and devices compile/link, but **callbacks still return
`-ENOTSUP`**. The meanings above are the interface contract, not implemented
runtime behavior. Chords will end only on the last Braille-key release; no
chord timeout property exists. The mode key selects a layer, not a character
table. Use an existing keymap layer number as its argument.

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
validate dynamically constructed rule/Studio bindings; the later input driver
must reject invalid runtime values as well. Parameterized macros should use
a valid dot number as their replaceable placeholder, not zero.

## Source layout and checks

- `dts/bindings/` and `dts/behaviors/braille.dtsi`: production schemas, default
  devices, and named per-keyboard default-mode configuration.
- `src/behavior_braille.c`: central-local ZMK device registration; unsupported
  callbacks until the input-routing leaf.
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
# With the pinned workspace present, also check the real Kconfig parser:
PYTHONPATH=build/baseline/zephyr/scripts/kconfig python3 tests/host/test_kconfig.py
# Production DT schemas + C assertions, using the same pinned workspace:
python3 -m venv build/bindings/venv
build/bindings/venv/bin/pip install PyYAML==6.0.3
build/bindings/venv/bin/python tests/host/test_devicetree.py
```

These check enabled/disabled source selection, optional Grade 2 isolation,
compilation with/without behavior nodes, central locality, and the explicit
unsupported callbacks. The host shim is not a Zephyr runtime and does not prove
hardware behavior. The devicetree tests run Zephyr's actual parser/header
generator, then compile production assertions against real Zephyr headers with
the host compiler (`CONFIG_ARCH_POSIX`); they test valid dots, rejected boundary
values, cell schemas, custom devices, extensible names, and the `ueb1` default.
Set `ZEPHYR_BASE` and `ZMK_APP` to use a different pinned workspace. They do not
run a Zephyr kernel. `tests/firmware/bindings.overlay` is a Paintbrush-central
compile/link fixture, not a usable keymap or a replacement for the later
both-keyboard smoke/CI matrix. The existing `tests/braille/*.yaml` files are
future conformance data, not an executable engine test suite yet.

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

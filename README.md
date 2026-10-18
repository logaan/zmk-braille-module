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

Production devicetree bindings and default nodes are not supplied yet. The
registration hooks recognize `zmk,behavior-braille-dot`,
`zmk,behavior-braille-space`, `zmk,behavior-braille-mode`, and
`zmk,behavior-braille-settings`; they are not a functional keymap API yet.

## Source layout and checks

- `src/behavior_braille.c`: central-local ZMK device registration; unsupported
  callbacks until the input-routing leaf.
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
```

These check enabled/disabled source selection, optional Grade 2 isolation,
compilation with/without behavior nodes, central locality, and the explicit
unsupported callbacks. The host shim is not a Zephyr runtime and does not prove
hardware behavior. The existing `tests/braille/*.yaml` files are future
conformance data, not an executable engine test suite yet.

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

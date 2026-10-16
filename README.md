# ZMK Braille module (hello-world skeleton)

An external ZMK feature module intended for six-dot Unified English Braille (UEB) uncontracted input. **Currently this is only a build/discovery smoke test:** when enabled, it logs `Hello from the ZMK Braille module!` at startup. It does not intercept keys, translate Braille, or send HID output. Normal keyboard behavior is unchanged.

## Try it locally

In a ZMK west workspace, pass this directory as an extra Zephyr module to a central or non-split firmware build (replace the paths and board with your own):

```sh
west build -s /path/to/zmk/app -b <board> -- \
  -DZEPHYR_EXTRA_MODULES=/path/to/braille-module \
  -DZMK_CONFIG=/path/to/zmk-config
```

Add `CONFIG_ZMK_FEATURE_BRAILLE=y` to the firmware's `.conf` file (for example, a shield `.conf` or the config directory's `config/<board>.conf`). Reconfigure/pristine-build after changing the setting. Enable ZMK logging and a log transport in your firmware configuration if you want to *see* the startup message; no keypress is needed. The option is unavailable on split peripherals, where input will eventually be processed by the central half.

The module is disabled by default. No device-tree nodes or keymap bindings are required for this stub. It has no extra west dependencies and can also be included in a west manifest later when it has a published remote.

See [the implementation plan](plans/00-implementation.md) for the intended input design, acceptance gates, and limitations. The unchanged [ICEB rulebook](references/README.md) is a separate reference under its own noncommercial/no-derivatives license; the MIT license in `LICENSE` applies to the module code, not the PDF.

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

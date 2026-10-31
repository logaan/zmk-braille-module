# zmk-braille-module

An out-of-tree [ZMK](https://zmkfirmware.dev) module that adds chorded
Perkins-style Braille typing to a keyboard. Ten Braille keys — dots 1–8, a
Braille space, and a braille-mode key — are chorded into characters, signs,
and ordinary keyboard commands. Output is real host keystrokes for a US
layout, never Unicode Braille. The module runs on a split keyboard's central
half (or a whole non-split board) and leaves ZMK core untouched; keyboards
adopt it through their `zmk-config` manifest alone.

## What it can do today

Computer8, contextual UEB1, optional contracted UEB2, persistent modes and
overrides, and on-device settings are implemented. Software tests and firmware
builds are recorded in [verification](docs/verification.md). Physical keyboard
and macOS/VoiceOver acceptance remain user-owned.

**Chord capture** — Braille keys are ordinary ZMK behaviors (`&bc 1`–`&bc 8`,
`&bspc`) usable on any layer. A chord is the nine-bit mask of every Braille
key held from first press to last release; there are no timers. Dot-only,
space-only, and space-plus-dot chords all flow through the same path.

**Three character modes**, selected by name:

- `ueb1` — contextual UEB Grade 1 covering all 95 printable US characters:
  lowercase letters, digits via the numeric indicator with digit runs and
  numeric spaces, capital letters plus capitalised word/passage indicators and
  the terminator, silent Grade 1 indicators, punctuation and symbols including
  21 ordered two-cell signs, and dots 2,3,6 read as an opening double quote at
  a word start and a question mark everywhere else.
- `computer8` — eight-dot computer Braille: every printable non-space ASCII
  cell, with dot 7 as Delete and dot 8 as Return. The mapping is VoiceOver's
  Liblouis US eight-dot table, recorded with provenance in
  `references/research/computer8.yaml`; reserved patterns send nothing and are
  available to user overrides.
- `ueb2` — optional contracted UEB reading (`CONFIG_ZMK_BRAILLE_GRADE2=y`),
  with 182 contraction mappings and 328 Appendix 1 longer-word entries from
  ICEB 2024. It buffers a word until Braille space, Return, or Tab. At 32 cells
  it resolves a synthetic word boundary; the next cell starts a new word.
  Numeric, capitals and Grade 1 indicators control uncontracted fallback.

**Factory commands** — 22 shared command chords per mode following macOS VoiceOver braille
conventions (sources noted in `src/braille_rules_nav.c`): Braille space,
arrows, Home/End, Page Up/Down, Tab, Return with dot-8 and space+dot-8
aliases, Escape, Delete, and one-shot sticky Shift, Control, Option, Command,
and Globe. With the settings editor enabled, the project-local space+123456
chord enters settings.

**Context and pending input** — the engine tracks numeric mode, capitals
scope, and word start, applying a per-class transition after every commit.
Matching always prefers the longest valid sequence with no timer: ambiguous
input stays pending until the next chord extends it or rules it out, a
fallback commits the longest complete match and reprocesses the remaining
cells exactly once, and an orphan prefix is dropped with a display-notice
hook. Whitespace chords commit pending text at a word end; other commands
discard it. Layer changes, non-Braille key presses, mode changes, and settings
entry reset context silently.

**Word positions and overrides** — rules can be constrained to word start,
middle, or end. One shared user-override layer stays active in every mode
with up to 256 saved identities. An override supports up to eight cells and
16 ordered actions. The compact RAM pool defaults to 16 KiB and flash space
provides a second bound: 256 small rules fit in RAM; many large rules can run
out of space sooner. Failed saves leave the previous live and saved rule intact.
Actions persist by compiled device name and parameters, never pointers.
Corrupt, unsupported or unavailable-behavior records are skipped independently
and retained for recovery; the display reports skipped records.

**Displays and settings** — the original Toucan Prospector dashboard shows both
board batteries, preferred/active USB or Bluetooth output, connection state,
effective modifiers, mode, currently held dots and context-aware text preview,
and the actual pending input buffer. Previewing emits no host input. The
[settings editor](docs/settings-editor.md) captures input, searches compiled
behaviors/keycodes, edits parameters, confirms saves, removes overrides and
resets only module overrides. [Small-display widgets](docs/small-display-widget.md)
serve the QBF OLED and standalone Toucan display.

## Scope and limitations

Output assumes a US host keyboard layout. Globe uses a consumer HID usage,
not a true Fn key; its host shortcuts need physical verification and require
`CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_FULL=y`. Other language tables, nonvisual
feedback, Touch ID, and power control are outside this implementation. No test
result here claims physical keyboard, Bluetooth pairing, panel readability,
power-cycle persistence, or VoiceOver acceptance.

## Cheatsheet

[`docs/braille-cheatsheet/braille-cheatsheet.pdf`](docs/braille-cheatsheet/braille-cheatsheet.pdf)
is the printable UEB quick-reference: lowercase, capitals, punctuation and
symbols, digits in both grades, keyboard chords, and the Grade 2 contraction
tables. It is a Braille reference; the exact implemented mappings and software
coverage are linked below. Sources live in
`docs/braille-cheatsheet/` and compile with Typst.

## Using it

1. Add the module to your zmk-config west manifest (see the `zmk-braille-module`
   project entries in the baseline keyboards' `config/west.yml`).
2. Enable `CONFIG_ZMK_FEATURE_BRAILLE=y` on the central half's firmware.
   Enable `CONFIG_ZMK_BRAILLE_GRADE2=y` for UEB2 and
   `CONFIG_ZMK_BRAILLE_STORAGE=y` for saved modes/overrides. With UEB2 or the
   display editor, set `CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE=6144` explicitly
   in the central configuration; board/application defaults can override a
   Kconfig default. The tested display profiles include this budget.
3. Include `behaviors/braille.dtsi` from your keymap and bind Braille keys:
   `&bc 1`–`&bc 8` for dots, `&bspc` for Braille space, `&bmode <layer>` for
   the braille-mode key (hold to chord on the target layer, tap to switch),
   and `&bsettings` for the settings-entry action.
4. Select character modes at runtime with `&bselect BRAILLE_MODE_COMPUTER8`,
   `&bselect BRAILLE_MODE_UEB1`, or `&bselect BRAILLE_MODE_UEB2`. The constants
   are included by `behaviors/braille.dtsi`. `&bselect BRAILLE_MODE_NEXT` cycles
   through the compiled modes (UEB1, Computer8, then optional UEB2). UEB2 needs
   `CONFIG_ZMK_BRAILLE_GRADE2=y`. Selection is independent of layers and
   discards pending input and any held chord; release all dots before typing
   in the new mode. An unavailable mode leaves the selection unchanged.
   With storage enabled, the selected mode is restored on boot. An absent or
   unavailable saved mode falls back to the configured default.
5. Optionally set the boot mode with `&braille { default-mode = "computer8"; };`
   in an overlay (defaults to `ueb1`).
6. For on-device editing enable `CONFIG_ZMK_BRAILLE_SETTINGS_UI=y` and its
   renderer: the Toucan dongle uses `toucan_braille_ui`; QBF and standalone
   Toucan use `CONFIG_ZMK_BRAILLE_WIDGET=y` with their custom-screen hook.
   Headless firmware rejects `&bsettings` instead of trapping typing.
   Use the [control table and workflow](docs/settings-editor.md).
7. Settings can select only compiled behaviors. To retain an otherwise unused
   behavior, put `#define ZMK_BEHAVIORS_KEEP_<NAME>` (or
   `ZMK_BEHAVIORS_KEEP_ALL`) before `#include <behaviors.dtsi>`.

A minimal Perkins row, usable on a QBF base layer or a Toucan Braille layer:

```dts
#include <behaviors.dtsi>
#include <behaviors/braille.dtsi>
#include <dt-bindings/zmk/keys.h>
/* Left little-to-index, then right index-to-little. */
/* &bc 7 &bc 3 &bc 2 &bc 1    &bc 4 &bc 5 &bc 6 &bc 8 */
/* A braille space is &bspc; conventional space remains &kp SPACE. */
/* &bmode 1 holds/toggles layer 1; &bselect BRAILLE_MODE_NEXT changes translation. */
```

Complete keymap examples are maintained in [QBF](https://github.com/logaan/qbf-paintbrushes/blob/main/basic-zmk/config/qbf_paintbrush.keymap)
and [Toucan](https://github.com/logaan/zmk-keyboard-toucan/blob/main/config/toucan.keymap).
The actual QBF base layer has 16 positions; Toucan has 42 positions and uses
layer 5 for Braille. Keep the board's existing matrix and complete binding
count, replacing only selected bindings with the examples above.

## Building and testing

Host unit and fixture tests (Python + a C compiler; the pinned ZMK keycode
checks need the baseline workspace):

```sh
uv run --no-project --with pyyaml --with kconfiglib \
  python -m unittest discover -s tests/host
```

One legacy suite (`test_coordinator.py`) fails for pre-existing, unrelated
reasons that reproduce from an older baseline commit; it is not evidence about
the module.

Native acceptance runs the real unmodified ZMK stack under Docker against the
pinned ZMK v0.3.0 workspace expected at `build/baseline`:

```sh
./scripts/test-commands      # factory commands, sticky modifiers, HID, UEB1/Computer8 characters
./scripts/test-routing       # key-edge routing versus recorded snapshots
./scripts/test-mode-settings # mode key and settings gate checkpoints
```

Firmware images for both baseline keyboards (no hardware is flashed):

```sh
./scripts/smoke-build qbf-left qbf-right qbf-computer8
# Use a separate west workspace initialized from Toucan config/west.yml:
WORKSPACE=/path/to/modern-workspace ./scripts/smoke-build toucan-left toucan-right
```

## Baseline hardware

- [QBF Paintbrushes](https://github.com/logaan/qbf-paintbrushes) — two
  Paintbrush PCBs on nice!nano v2 as a wireless split, 16 keys, an OLED on
  each half; Braille keys on the base layer.
- [beekeeb Toucan](https://github.com/logaan/zmk-keyboard-toucan) —
  `xiao_ble//zmk` split with a 144×168 display, primarily QWERTY with a
  braille-mode key. The optional 240×280 Prospector dongle is the central in
  its three-device profile.

Both keyboards pin this module's revision in their own `config/west.yml` and
build it through their normal CI. QBF and native fixtures use ZMK v0.3.0;
Toucan pins `4dd95d7840eb3d5a3f59bb29fbf1ab054715ef85` with Zephyr 4.1.
Do not mix these dependency sets in one west workspace.

## Mapping and acceptance references

- [UEB1 symbols and provenance](references/research/ueb-literary-signs.yaml)
- [All Computer8 mapped/reserved patterns](references/research/computer8.yaml)
- [UEB2 contractions](references/research/ueb-grade2-contractions.yaml) and
  [Appendix 1 longer words](references/research/ueb-grade2-shortforms.json)
- [Factory command chords and sources](src/braille_rules_nav.c)
- [Shared US keyboard coverage](references/key-coverage.md)
- [ICEB edition, provenance and license](references/README.md)
- [Software evidence and physical acceptance checklist](docs/verification.md)

## Repository map

- `src/`, `include/` — capture, matching engine, dispatch, mode registry,
  event boundaries, and the per-mode rule tables.
- `dts/` — the `zmk,braille` configuration node and Braille behavior bindings.
- `tests/` — host unit tests, YAML acceptance fixtures, and native ZMK
  firmware fixtures.
- `references/` — the ICEB rulebook provenance and license
  (`references/README.md`; the PDF itself is not committed), UEB and Computer8
  research records, and the shared key-coverage checklist
  (`references/key-coverage.md`).
- `tasks/` — TaskDown task history.

## License

Module code is MIT (`LICENSE`). The ICEB rulebook PDF is redistributed
unchanged under its own Creative Commons BY-NC-ND 2.5 Australia license —
see `references/README.md` before redistributing it.

# zmk-braille-module

An out-of-tree [ZMK](https://zmkfirmware.dev) module that adds chorded
Perkins-style Braille typing to a keyboard. Ten Braille keys — dots 1–8, a
Braille space, and a braille-mode key — are chorded into characters, signs,
and ordinary keyboard commands. Output is real host keystrokes for a US
layout, never Unicode Braille. The module runs on a split keyboard's central
half (or a whole non-split board) and leaves ZMK core untouched; keyboards
adopt it through their `zmk-config` manifest alone.

## What it can do today

Software-complete through phase 04. Physical testing on real hardware is the
user's acceptance step and has not been performed yet.

**Chord capture** — Braille keys are ordinary ZMK behaviors (`&bc 1`–`&bc 8`,
`&bspc`) usable on any layer. A chord is the nine-bit mask of every Braille
key held from first press to last release; there are no timers. Dot-only,
space-only, and space-plus-dot chords all flow through the same path.

**Two character modes**, selected by name:

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

**Factory commands** — 22 chords per mode following macOS VoiceOver braille
conventions (sources noted in `src/braille_rules_nav.c`): Braille space,
arrows, Home/End, Page Up/Down, Tab, Return with dot-8 and space+dot-8
aliases, Escape, Delete, and one-shot sticky Shift, Control, Option, Command,
and Globe.

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
(in memory for now; on-device editing and persistence are phase 05 work).

## What it does not do yet

OLED/display feedback, the on-device settings interface, persistence of the
selected mode and overrides (phase 05), final integration documentation
(phase 06), and UEB Grade 2 contractions (phase 07; a placeholder translation
unit exists behind `CONFIG_ZMK_BRAILLE_GRADE2`). Nothing here claims physical
keyboard or VoiceOver acceptance.

## Cheatsheets

- [`references/braille-cheatsheet.pdf`](references/braille-cheatsheet.pdf) —
  the implementation cheatsheet: every implemented mapping with its ICEB rule
  citations and verification boundary. Sources: `references/braille-cheatsheet.typ`.
- [`docs/braille-cheatsheet/braille-cheatsheet.pdf`](docs/braille-cheatsheet/braille-cheatsheet.pdf) —
  a printable UEB quick-reference (all of Grade 1 and Grade 2, including
  contractions the module does not implement yet). Sources: `docs/braille-cheatsheet/`.

## Using it

1. Add the module to your zmk-config west manifest (see the `zmk-braille-module`
   project entries in the baseline keyboards' `config/west.yml`).
2. Enable `CONFIG_ZMK_FEATURE_BRAILLE=y` on the central half's firmware.
3. Include `behaviors/braille.dtsi` from your keymap and bind Braille keys:
   `&bc 1`–`&bc 8` for dots, `&bspc` for Braille space, `&bmode <layer>` for
   the braille-mode key (hold to chord on the target layer, tap to switch),
   and `&bsettings` for the settings-entry action.
4. Optionally set the boot mode with `&braille { default-mode = "computer8"; };`
   in an overlay (defaults to `ueb1`).

## Building and testing

Host unit and fixture tests (Python + a C compiler; the pinned ZMK keycode
checks need the baseline workspace):

```sh
uv run --no-project --with pyyaml --with kconfiglib \
  python -m unittest discover -s tests/host
```

Two legacy suites (`test_cheatsheet.py`, `test_coordinator.py`) fail for
pre-existing, unrelated reasons that reproduce from an older baseline commit;
they are not evidence about the module.

Native acceptance runs the real unmodified ZMK stack under Docker against the
pinned ZMK v0.3.0 workspace expected at `build/baseline`:

```sh
./scripts/test-commands      # factory commands, sticky modifiers, HID, UEB1/Computer8 characters
./scripts/test-routing       # key-edge routing versus recorded snapshots
./scripts/test-mode-settings # mode key and settings gate checkpoints
```

Firmware images for both baseline keyboards (no hardware is flashed):

```sh
./scripts/smoke-build qbf-left qbf-right qbf-computer8 toucan-left toucan-right toucan-computer8
```

## Baseline hardware

- [QBF Paintbrushes](https://github.com/logaan/qbf-paintbrushes) — two
  Paintbrush PCBs on nice!nano v2 as a wireless split, 16 keys, an OLED on
  each half; Braille keys on the base layer.
- [beekeeb Toucan](https://github.com/logaan/zmk-keyboard-toucan) —
  seeeduino_xiao_ble split with a nice!view; primarily QWERTY with a
  braille-mode key.

Both keyboards pin this module's revision in their own `config/west.yml` and
build it through their normal CI. The module and both keyboards use one pinned
ZMK release (v0.3.0).

## Repository map

- `src/`, `include/` — capture, matching engine, dispatch, mode registry,
  event boundaries, and the per-mode rule tables.
- `dts/` — the `zmk,braille` configuration node and Braille behavior bindings.
- `tests/` — host unit tests, YAML acceptance fixtures, and native ZMK
  firmware fixtures.
- `references/` — the pinned ICEB rulebook PDF with provenance and license
  (`references/README.md`), UEB and Computer8 research records, and the shared
  key-coverage checklist (`references/key-coverage.md`).
- `tasks/` — TaskDown task history.

## License

Module code is MIT (`LICENSE`). The ICEB rulebook PDF is redistributed
unchanged under its own Creative Commons BY-NC-ND 2.5 Australia license —
see `references/README.md` before redistributing it.

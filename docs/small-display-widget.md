# Small central displays

`CONFIG_ZMK_BRAILLE_WIDGET=y` adds a reusable overlay on a keyboard's existing
status screen. It requires the settings editor and a display. Call
`braille_widget_init(screen, width, height, rotation)` after creating the normal
widgets. The overlay is shown on `CONFIG_ZMK_BRAILLE_WIDGET_LAYER` or whenever
the editor is active; other layers retain the normal status screen.

The QBF central uses its real physical orientation: 32 pixels across and 128
down. The canvas is rotated into the controller's 128×32 framebuffer, following
the existing clockwise/anticlockwise option. The standalone Toucan uses a
144×168 text overlay on braille layer 5. Peripheral screens remain unchanged.

The narrow OLED uses one-row stage codes such as SAVE, ACT, KEY, and MODE,
leaving fourteen rows for content. Wider displays retain full screen titles.
Settings show the selected item first, and the input/query or
confirmation context. Long content advances through numbered pages every four
seconds; changing the selected item returns to page one. Every action name and
parameter remains available for review. Unselected menu rows are omitted on
these small displays. Controls are shared with the editor: Braille Space plus
dot 1/dot 4 moves; Space+15 activates; Space+12 goes back. The factory settings
entry is Space+123456. See the settings guide for editing and exit controls.

Outside settings, the view shows mode, the held-chord preview, held dots, and the
pending count and newest buffered cells. The count makes a shortened long
buffer explicit. Reserved cells, dropped prefixes, and skipped saved records
produce brief notices. Input inspection runs on the serialized input queue;
LVGL renders a copied snapshot on its display queue. No preview invokes actions.

Host tests cover selected-item/context pagination and buffer formatting. The
LVGL8 preview harness renders the actual canvas helper and checks every rotated
pixel against the portrait buffer. Firmware builds and physical readability,
scroll timing and power-cycle checks are separate validation steps.

Reproduce the software checks from the module root:

```
python3 tests/host/test_small_screen.py
./scripts/preview-small-display
```

The preview uses the pinned LVGL8 checkout in `build/baseline`; `--lvgl` can
select another LVGL8 checkout. It writes `build/previews/small-display.png`,
showing every page side by side in the physical reading orientation.

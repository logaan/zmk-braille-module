# braille-cheatsheet — glm-5.3-flash edition

Two-page A4 PDF reference built from the same information content as
`references/braille-cheatsheet.typ`, laid out with a different design.

Design choices:

- **Braille is drawn as vector dot grids**, not font glyphs — every cell is a
  2×3 (or 2×4 for dots 7/8) grid of filled circles. This renders identically on
  any machine with no braille-font dependency, shows blank positions in the
  legend, and lets dot numbers appear under the letter cells.
- **Dots 7/8 are drawn in rose** so 8-dot keys (Computer-8 capitals, factory
  chords) are visually distinct from literary 6-dot cells.
- A4 page size, zebra-striped tables, color-coded sections
  (teal UEB 1 · orange Computer 8 · violet UEB 2 · rose factory chords).
- Data is loaded at compile time from the same verified research YAMLs the
  original sheet uses (`references/research/*.yaml`), so both documents share
  one source of truth.

## Rebuild

From the repository root:

```sh
typst compile --root . references/braille-cheatsheet/glm-5.3-flash/braille-cheatsheet.typ
```

(`--root .` is required because the file reads `../../research/*.yaml`; the
PDF is written next to this README. Quick previews:
`typst compile --root ../.. --format png --ppi 100 braille-cheatsheet.typ pg-{p}.png`.)

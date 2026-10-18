# Braille input · two-page A4 reference

Open [`braille-cheatsheet.pdf`](braille-cheatsheet.pdf). It is **two A4 portrait pages** (210 × 297 mm). All fonts are embedded; no fonts or software need to be installed to read or print the PDF. For printing, use **A4, actual size / 100%**; for duplex printing, flip on the long edge.

## Design

- **Page 1:** shared lowercase letters, side-by-side UEB 1 / Computer8 character mappings and examples, quotation marks, and factory navigation/editing chords.
- **Page 2:** all **182** optional UEB 2 contractions, capitalisation and Grade 1 examples, and sticky-modifier/media chords.
- Atkinson Hyperlegible body text, monospaced character labels, aligned mappings, restrained row shading, and separate colours for the two character modes.
- Braille cells are **vector drawings**, with larger dark active dots and small pale position guides. The spacing clearly separates successive cells. Each cell has pressed-dot alt text in the tagged PDF.
- Command chords use explicit **Space + dot numbers** notation, distinct from sequential literary cells. A dot-position guide is on page 1.
- Planned-support status, hardware-acceptance caveats, missing defaults, sticky timing, Globe/Fn distinction, and source links are retained.

The original [`../../braille-cheatsheet.typ`](../../braille-cheatsheet.typ) is used **only as an information source**, including the research data it loads. Its layout and original PDF are not reused or modified. This is a separate design by `gpt-6.1-sol`.

## Rebuild and validate

From the repository root:

```sh
python3 references/braille-cheatsheet/gpt-6.1-sol/build.py
```

Requirements: Python 3.9+, Typst 0.15+, Poppler (`pdftotext`), and the **Atkinson Hyperlegible** and **Atkinson Hyperlegible Mono** fonts. No third-party Python packages are needed.

The build extracts the source's lookup and command metadata into `source-content.json`, compiles this directory's independent Typst design, then checks:

1. Every displayed mapping against the source: **376 mapping rows**, including all 182 contractions, plus **25 factory chords**.
2. Exactly **two A4 pages**.
3. Extractable text boxes for overlaps and print-margin overflow.

The source snapshot is committed so the design can also be compiled directly, without refreshing its information:

```sh
typst compile references/braille-cheatsheet/gpt-6.1-sol/braille-cheatsheet.typ
```

Check the existing snapshot and PDF without rewriting either:

```sh
python3 references/braille-cheatsheet/gpt-6.1-sol/build.py --check
```

Explanatory prose is authored in the design file; review it if the original source's notes change. The finished PDF was also rendered and visually inspected on both pages; its fonts are embedded and its braille artwork contains no raster images.

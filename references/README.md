# Normative UEB reference

The source of truth is [The Rules of Unified English Braille, Third Edition 2024](iceb/rules-of-unified-english-braille-2024.pdf), published by the International Council on English Braille (ICEB), edited by Matthew Horspool (third edition). ICEB identifies it as the definitive UEB rulebook. The original, unmodified PDF is included here for reference; it covers both grades, so the implementation selects uncontracted requirements using sections 2.5 and 5.11 rather than assuming the entire book is grade 1 only.

## Provenance

- Publisher page: <https://iceb.org/ueb.html> (redirects to the current UEB page).
- Download: <https://iceb.org/wp-content/uploads/2025/10/Rules-of-Unified-English-Braille-2024.pdf>.
- Retrieved: 2026-09-26.
- Edition: Third Edition 2024; electronic print ISBN 978-1-7383661-0-1.
- SHA-256: `bf742499ee2eb3cbdf4914d97bbf0b2857bd3e91621f04a4990091b64e5a105e`.
- Copyright: Third Edition © 2024, International Council on English Braille. Earlier-edition credits are preserved in the PDF.
- Modifications: none. This is the downloaded PDF byte for byte.

## Reference license

The copyright page (printed page ii, PDF page 2) licenses the publication under **Creative Commons Attribution–Noncommercial–No Derivative Works 2.5 Australia**: <https://creativecommons.org/licenses/by-nc-nd/2.5/au/>. The [legal code](https://creativecommons.org/licenses/by-nc-nd/2.5/au/legalcode) governs reuse. Preserve attribution and the license; redistribute this reference unchanged and only for noncommercial purposes under that license. Its terms apply to this PDF, independently of any eventual module code license. Inclusion does not imply ICEB endorsement. Do not bundle the reference into a commercial distribution under an assumed code license.

## Updates and scope

At retrieval, ICEB's UEB page listed the 2024 third edition and no separately labelled rulebook errata. It also listed separate Guidelines for Technical Material (GTM), their errata, and approved replacement sections. In particular, [GTM 1.7: Choice and Placement of Grade 1 Indicators](https://iceb.org/wp-content/uploads/2026/02/GTM-1.7-Grade-1-Indicators-Approved.pdf) was approved 2025-07-14. That is a technical-guidelines supplement, not a replacement grade 1 rulebook. Review applicable GTM updates before implementing technical notation. The present literary-input baseline is the pinned rulebook; recheck the publisher before changing the baseline and record any adopted updates explicitly.

For local text searching, `pdftotext -layout iceb/rules-of-unified-english-braille-2024.pdf /tmp/ueb-rules.txt` is useful. Treat the original PDF as authoritative: text extraction can damage Braille cells and layout. Do not commit modified derivatives of the licensed reference.

## Braille cheatsheet

[`braille-cheatsheet.pdf`](braille-cheatsheet.pdf) is a two-page, result-first lookup for the **planned** character mappings, not a statement of current firmware support. Its tables cover printable UEB 1 and Computer8 characters, complete capital/digit sequences, and all 182 optional UEB 2 contractions. A visible `␣` represents a space result; `Space` in a chord sequence is the Braille Space key. The two pages also show all 25 software-verified Phase-02 factory rules, including their Delete/Return aliases, sticky modifiers and supported volume/playback actions; physical hardware acceptance is still pending. `Space +` a cell means one simultaneous chord, unlike adjacent literary cells. Per user instruction, keys without a sourced assignment are skipped and listed as omissions; settings entry remains deferred.

Build from the repository root with Typst 0.15+ and the Avenir, Apple Braille, and DejaVu Sans Mono fonts installed:

```sh
typst compile references/braille-cheatsheet.typ
python3 -m unittest discover -s tests/host -p test_cheatsheet.py
```

The reference tests require PyYAML and Typst. They compare the displayed mappings with the existing fixtures and verified research; Poppler's `pdftotext` additionally checks the two-page layout for overlapping text and margin overflow. UEB tables and factory commands are read directly from `research/`; the Computer8 mappings are checked against `tests/braille/computer8/characters.yaml`. Command metadata is checked against `research/factory-command-chords.yaml`. Update `braille-cheatsheet.typ` and rebuild the PDF before each implementation commit.

# Braille Input Reference Cheatsheet (gemini-3.8-flash)

A publication-grade, 2-page A4 reference sheet for the ZMK Braille Module, covering planned character mappings and direct factory chords for Unified English Braille (UEB Grade 1 & Grade 2) and Computer8 (Liblouis `en-us-comp8`).

## Files

- `braille-cheatsheet.pdf`: Compiled 2-page A4 PDF reference.
- `braille-cheatsheet.typ`: Typst 0.15 source document.
- `data.json`: Self-contained JSON dataset extracted directly from verified research YAMLs (`factory-command-chords.yaml`, `ueb-literary-signs.yaml`, `ueb-capitalisation.yaml`, `ueb-numeric-mode.yaml`, `ueb-grade2-contractions.yaml`).

## Design & Readability Highlights

1. **High-Contrast Ghost Dot Matrix**:
   - Rather than isolated dots floating on empty space (which makes determining dot position difficult), every braille cell is rendered with active dots in deep slate (`#0f172a`) and inactive ghost dots in subtle slate (`#cbd5e1`).
   - Position within the 6-dot or 8-dot cell is immediately clear at a glance.
   - Computer8 mappings use full 8-dot matrices (dots 1–8), making dot 7 (capitalization) and dot 8 instantly identifiable.
2. **Typography**:
   - Typeset in **Atkinson Hyperlegible** (developed by the Braille Institute of America for maximum legibility) and **Atkinson Hyperlegible Mono** for code and symbol tokens.
3. **Information Organization**:
   - **Page 1: Core Character Sets & Factory Navigation**:
     - 6-dot and 8-dot reference diagrams with numbered dot positions.
     - Lowercase letters (a–z) and Braille Space (shared base).
     - Side-by-side comparison of **UEB Grade 1** (sequential cells) and **Computer8** (direct 8-dot chords):
       - Uppercase letters (A–Z)
       - Digits (0–9)
       - Punctuation & ASCII keyboard symbols (32 printable characters)
       - Quotation marks (opening, closing, nondirectional)
       - Syntax, capitalisation, and numeric mode examples
       - Real-world text and code snippets
     - 17 Factory navigation, cursor, and editing chords (Mac VoiceOver standard).
   - **Page 2: UEB Grade 2 Contractions & Factory Modifiers**:
     - All 182 normative contractions across all classes:
       - Alphabetic wordsigns (23 whole words)
       - Strong wordsigns (6 whole words)
       - Lower wordsigns (6 whole words)
       - Strong contractions (5 words & word-parts)
       - Strong groupsigns (12 inside-word signs)
       - Lower groupsigns (10 signs: prefix, medial, and anywhere)
       - Initial-letter contractions (33 two-cell signs)
       - Final-letter groupsigns (12 post-letter signs)
       - Shortforms (75 specially abbreviated words in a 5-column table)
     - Grade 1 indicators (symbol, word, passage) and capitalisation rules.
     - 8 Factory sticky modifiers and media actions (Mac VoiceOver & Android TalkBack standards).
     - VoiceOver sequential chording compositions (Line Start, Line End, Previous/Next UI Item).

## Compilation

To rebuild the PDF from this directory or repository root:

```bash
typst compile references/braille-cheatsheet/gemini-3.8-flash/braille-cheatsheet.typ references/braille-cheatsheet/gemini-3.8-flash/braille-cheatsheet.pdf
```

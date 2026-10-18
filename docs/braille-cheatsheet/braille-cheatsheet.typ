// Build: typst compile docs/braille-cheatsheet/braille-cheatsheet.typ
// Data, styling, cells, mnemonics, tables, and validation live in sibling modules.
#import "style.typ": cheatsheet
#import "data.typ": lowercase, digit-ueb1, digit-eight, symbols, capitals, chords, ueb2
#import "cells.typ": output, sequence
#import "tables.typ": character-section, digit-section, section-table, chord-section, word-section

#show: cheatsheet

// Sections flow naturally through the page columns; their table headers repeat
// on continuation. Capitals follows Lowercase as an Output/Input table, Symbols
// fills the next column, and the digit tables and keyboard chords start a fresh one.
#character-section("Lowercase", "Lowercase letters", lowercase)
#section-table("Capitals", "Capitals", ("ueb1", "eight-dot"), capitals,
  example => (output(example.example, code: true), sequence(example.ueb1, wrap: true)),
  columns: 2, headers: ("Output", "Input"), row: auto,
  intro: text(size: 8.2pt)[8 dot: add dot 7 to each uppercase letter.],
  inset: (x: 2pt, y: 0.2pt),
)
#character-section("Symbols", "Punctuation & symbols", symbols)
#colbreak(weak: true)
#digit-section("Digits · UEB 1", digit-ueb1)
#digit-section("Digits · 8 dot", digit-eight)
#chord-section("Navigation and Editing", chords)
#pagebreak(weak: true)
#for section in ueb2 { word-section(section) }

#include "validation.typ"

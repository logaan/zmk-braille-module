// Build: typst compile docs/braille-cheatsheet/braille-cheatsheet.typ
// Data, styling, cells, mnemonics, tables, and validation live in sibling modules.
#import "style.typ": cheatsheet, teal, blue
#import "data.typ": lowercase, digit-ueb1, digit-eight, symbols, capitals, chords, ueb2
#import "cells.typ": output, sequence, key-box
#import "tables.typ": character-section, digit-section, section-table, chord-section, word-section

#show: cheatsheet

// Sections flow naturally through the page columns; their table headers repeat
// on continuation. Capitals follows Lowercase as an Output/Input table and
// Symbols, the digit tables, and the keyboard chords follow without a break.
#character-section("Lowercase", "Lowercase letters", lowercase)
#section-table("Capitals", "Capitals", ("ueb1", "eight-dot"), capitals,
  example => (output(example.example, code: true), sequence(example.ueb1, wrap: true)),
  columns: 2, headers: ("Output", "Input"), row: auto,
  intro: text(size: 8.2pt)[#key-box[8 Dot] Add dot 7 to each uppercase letter.],
  inset: (x: 2pt, y: 0.2pt),
)
#character-section("Symbols", "Punctuation & symbols", symbols)
#digit-section("Digits · UEB Grade 1", digit-ueb1)
#digit-section("Digits · 8 dot", digit-eight)
#linebreak()
#align(center, text(size: 18pt, weight: "bold", fill: teal)[Quick Braille Fox])
#align(center, sequence("⠟⠅ ⠃⠗⠇ ⠋⠕⠭", space: [~~~]))
#colbreak()
#chord-section("Navigation and Editing", chords)
#for section in ueb2 { word-section(section) }
#linebreak()
#align(center, text(size: 18pt, weight: "bold", fill: blue)[Quick Braille Fox])
#align(center, sequence("⠟⠅ ⠃⠗⠇ ⠋⠕⠭", space: [~~~]))

#include "validation.typ"

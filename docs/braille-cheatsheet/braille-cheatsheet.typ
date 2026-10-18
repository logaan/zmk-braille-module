// Build: typst compile docs/braille-cheatsheet/braille-cheatsheet.typ
// Data, styling, cells, mnemonics, tables, and validation live in sibling modules.
#import "style.typ": cheatsheet
#import "data.typ": lowercase, digit-ueb1, digit-eight, symbols, capitals, chords, ueb2
#import "tables.typ": character-section, digit-section, section-table, capital-example, chord-section, word-section

#show: cheatsheet

// Sections flow naturally through the page columns; their table headers repeat
// on continuation. Capitals follows Lowercase, Symbols fills the next column,
// and the digit tables and keyboard chords start a fresh one.
#character-section("Lowercase", "Lowercase letters", lowercase)
#section-table("Capitals", "Capitals", ("ueb1", "eight-dot"), capitals,
  example => (capital-example(example),), columns: 1, row: auto,
  intro: text(size: 8.2pt)[8 dot: add dot 7 to each uppercase letter.],
)
#character-section("Symbols", "Punctuation & symbols", symbols)
#colbreak(weak: true)
#digit-section("Digits · UEB 1", digit-ueb1)
#digit-section("Digits · 8 dot", digit-eight)
#chord-section("Navigation and Editing", chords)
#pagebreak(weak: true)
#for section in ueb2 { word-section(section) }

#include "validation.typ"

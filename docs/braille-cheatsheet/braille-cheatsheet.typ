// Build: typst compile docs/braille-cheatsheet/braille-cheatsheet.typ
// Data, styling, cells, mnemonics, tables, and validation live in sibling modules.
#import "style.typ": cheatsheet
#import "data.typ": lowercase, digits, symbols, capitals, chords, ueb2
#import "tables.typ": character-section, section-table, capital-example, chord-section, word-section

#show: cheatsheet

// Sections flow naturally through the page columns; their table headers repeat
// on continuation. Capitals follows Lowercase.
#character-section("Lowercase", "Lowercase letters", lowercase)
#section-table("Capitals", "Capitals", ("ueb1", "eight-dot"), capitals,
  example => (capital-example(example),), columns: 1, row: auto,
  intro: text(size: 8.2pt)[8 dot: add dot 7 to each uppercase letter.],
)
#character-section("Digits", "Digits", digits)
#character-section("Symbols", "Punctuation & symbols", symbols)
#chord-section("Navigation and Editing", chords)
#for section in ueb2 { word-section(section) }

#include "validation.typ"

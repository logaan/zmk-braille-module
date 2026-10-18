// Build: typst compile docs/braille-cheatsheet/braille-cheatsheet.typ
// Data, styling, cells, mnemonics, tables, and validation live in sibling modules.
#import "style.typ": cheatsheet
#import "data.typ": lowercase, digits, symbols, capitals, navigation, modifiers, ueb2
#import "tables.typ": character-section, section-table, capital-example, chord-section, word-section

#show: cheatsheet

// Sections flow naturally through the page columns; their table headers repeat
// on continuation. Capitals keeps its own example layout without mnemonics.
#heading(level: 1)[Characters & keyboard chords]
#v(5pt)
#character-section("Lowercase", "Lowercase letters & Braille Space", lowercase)
#character-section("Digits", "Digits", digits)
#character-section("Symbols", "Punctuation & symbols", symbols)
#section-table("Capitals", "Capitals", ("ueb1", "eight-dot"), capitals,
  example => (capital-example(example),), columns: 1, row: auto,
  intro: text(size: 8.2pt)[8 dot: add dot 7 to each uppercase letter.],
)
#chord-section("Navigation and Editing", navigation)
#chord-section("Sticky modifiers", modifiers, headers: ("Modifier", "Input"))
#heading(level: 1)[UEB Grade 2]
#v(5pt)
#for section in ueb2 { word-section(section) }

#include "validation.typ"

#import "data.typ": *
#import "cells.typ": dot-mask, chord-cell
#import "mnemonics.typ": cell-mnemonics, mnemonic-spelling
#import "style.typ": page-margins

// Validate the data at compilation time, without a separate build program.
#assert(alphabet.len() == 26 and digits.len() == 10 and symbols.len() == 32)
#assert(characters.map(entry => entry.at(0)).sorted() == range(33, 127)
  .filter(code => not (65 <= code and code <= 90)).map(str.from-unicode))
#assert(symbols.find(entry => entry.at(0) == "\"").at(1) == "⠠⠶")
#assert(symbols.find(entry => entry.at(0) == "'").at(1) == "⠄")
#assert(ueb2.map(section => section.entries.len()).sum() == 182)
#assert(navigation.len() == 17 and modifiers.len() == 5)
#assert(chords.len() == 22 and chords.slice(0, navigation.len()) == navigation)
#assert(chords.slice(navigation.len()).all(entry =>
  entry.at(0).ends-with(" _") and entry.at(2) and entry.at(1).contains("space")))
#assert(capitals.len() == 7)
#assert(dot-mask((2, 3, 4, 5)) == 30)
#assert(cell-mnemonics.len() == 36)
#assert(mnemonic-spelling("⠃") == "b")
#assert(mnemonic-spelling("⠆") == "↓b")
#assert(mnemonic-spelling("⠐⠑") == "⠐e")
#assert(mnemonic-spelling("⠇⠇") == "ll")
#assert(mnemonic-spelling("⠨⠑") == "⠨e")
#assert(mnemonic-spelling("⠆⠉") == "↓bc")
#assert(ueb2.filter(section => section.at("headers", default: ()) != ())
  .map(section => (section.title, section.headers)) == (
  ("Strong wordsigns", ("Output", "Input")),
  ("Strong contractions", ("Output", "Input")),
  ("Strong groupsigns", ("Output", "Input")),
))
#assert(mnemonic-spelling(chord-cell(("space", 2, 3, 4, 5))) == "t")
#assert(mnemonic-spelling("⠡") == none)

// Every data row must be rendered exactly once, and every continued heading
// must start at the top of its column.
#context {
  let sections = query(<reference-section>)
  let rows = query(<reference-row>)
  if rows.len() > 0 {
    let expected = characters.len() + capitals.len() + navigation.len() + modifiers.len() + ueb2.map(section => section.entries.len()).sum()
    assert(rows.len() == expected, message: "Missing or duplicated reference rows")
    assert(sections.len() == ueb2.len() + 5, message: "Missing or duplicated sections")
    for section in sections {
      let data = section.value
      let actual = rows.filter(row => row.value.section == data.title)
      assert(actual.map(row => row.value.index) == range(data.entries.len()))
      let has-mnemonics = if data.title == "Navigation and Editing" { true }
        else if data.modes.contains("ueb2") {
          data.entries.any(entry => mnemonic-spelling(entry.at(1)) != none)
        } else { false }
      if has-mnemonics {
        assert(data.columns == 3 and data.headers == ("Output", "Input", "Mnemonic"),
          message: "Mnemonic tables need Output / Input / Mnemonic columns")
      } else if data.headers.len() > 0 {
        assert(data.columns == data.headers.len(),
          message: "Every labeled table needs one column per header")
      }
    }
    let chord-pages = query(<reference-chord>).map(chord => chord.location().page())
    assert(chord-pages.all(page => page == chord-pages.first()),
      message: "Keyboard chords and sticky modifiers must stay on one page")
    let fragments = query(<reference-fragment>)
    let bottoms = query(<reference-table-bottom>)
    assert(query(table).all(table => table.columns.all(width => width == 1fr)),
      message: "All columns within a table must have equal width")
    for section in sections {
      let parts = fragments.filter(fragment => fragment.value.section == section.value.title)
      let ends = bottoms.filter(bottom => bottom.value.section == section.value.title)
      assert(ends.len() == parts.len(), message: "Every table fragment needs a bottom rule")
      assert(parts.map(part => part.value.part) == range(1, parts.len() + 1))
      assert(parts.all(part => part.value.heading == if part.value.part == 1 {
        section.value.title
      } else {
        section.value.continuation
      }))
    }
    for fragment in fragments.filter(fragment => fragment.value.part > 1) {
      assert(calc.abs(fragment.location().position().y - page-margins.top) < 0.1pt,
        message: "A continued heading must begin its page column")
    }
  }
}

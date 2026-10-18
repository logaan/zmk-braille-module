#import "data.typ": alphabet
#import "cells.typ": braille, space-key, output
#import "style.typ": muted

// Derive letter mnemonics from the actual cells, not from the output word.
// The first ten letters use only the top two rows, so shifting their masks
// one bit lowers every dot by exactly one row: a–j become the lower signs.
#let cell-mnemonics = {
  let spellings = (:)
  for (letter, cell, _) in alphabet { spellings.insert(cell, letter) }
  for (letter, cell, _) in alphabet.slice(0, 10) {
    let lowered = str.from-unicode(0x2800 + 2 * (cell.to-unicode() - 0x2800))
    spellings.insert(lowered, "↓" + letter)
  }
  spellings
}
#let mnemonic-spelling(cells) = {
  let spelling = cells.clusters().map(cell => cell-mnemonics.at(cell, default: cell)).join()
  if spelling != cells { spelling } else { none }
}

// Keep non-letter signs as vector braille, including contraction prefixes.
// A down arrow applies only to the immediately following letter.
#let mnemonic(cells, space: false) = {
  let spelling = mnemonic-spelling(cells)
  if spelling == none {
    text(fill: muted)[—]
  } else {
    let tokens = if space { (space-key, text(size: 8pt, fill: muted)[+]) } else { () }
    let letters = ""
    for character in spelling.clusters() {
      if 0x2800 <= character.to-unicode() and character.to-unicode() <= 0x28ff {
        if letters != "" {
          tokens.push(output(letters, code: true, size: 8.7pt))
          letters = ""
        }
        tokens.push(braille(character))
      } else {
        letters += character
      }
    }
    if letters != "" { tokens.push(output(letters, code: true, size: 8.7pt)) }
    [#metadata((input: cells, spelling: spelling, space: space)) <reference-mnemonic>]
    box(grid(columns: (auto,) * tokens.len(), column-gutter: 2pt, align: horizon, ..tokens))
  }
}

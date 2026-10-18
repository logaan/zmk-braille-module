// Build and validate with: python3 docs/braille-cheatsheet/build.py
#let source = json("source-content.json")
#let ink = rgb("#172e37")
#let muted = rgb("#4f6269")
#let teal = rgb("#006b68")
#let rule = rgb("#d6e1e3")
#let wash = rgb("#f0f5f5")
#let mono = "Atkinson Hyperlegible Mono"

#set document(
  title: "Braille input — two-page A4 quick reference",
  author: "Braille module",
  description: "UEB 1 and 8 dot character mappings, UEB 2 contractions, and factory chords.",
)
#set page(
  paper: "a4",
  margin: (left: 11mm, right: 11mm, top: 10mm, bottom: 13mm),
  header: none,
  footer: none,
  numbering: none,
)
#set text(
  font: "Atkinson Hyperlegible", size: 9.4pt, fill: ink,
  top-edge: "ascender", bottom-edge: "descender", lang: "en",
)
#set par(leading: 0pt, spacing: 0pt)
#set heading(numbering: none)
#show heading: set block(above: 0pt, below: 0pt)

#let find(group, mode) = source.lookups.find(row => row.group == group and row.modes.contains(mode)).entries
#let register(group, entries, modes) = [#metadata((group: group, entries: entries, modes: modes)) <reference-lookup>]

// Vector cells use the same dot geometry throughout the document.
#let braille(cell, eight: false) = {
  let mask = cell.to-unicode() - 0x2800
  let order = if eight { (1, 2, 3, 7, 4, 5, 6, 8) } else { (1, 2, 3, 4, 5, 6) }
  let rows = if eight { 4 } else { 3 }
  let height = if eight { 12.4 } else { 9.2 }
  let circles = order.enumerate().map(((i, dot)) => {
    let on = calc.rem(calc.floor(mask / calc.pow(2, dot - 1)), 2) == 1
    let x = if i < rows { 1.6 } else { 4.8 }
    let y = 1.35 + calc.rem(i, rows) * 3.2
    let r = if on { "0.9" } else { "0.35" }
    let fill = if on { "#172e37" } else { "#b9c8cd" }
    "<circle cx='" + str(x) + "' cy='" + str(y) + "' r='" + r + "' fill='" + fill + "'/>"
  }).join()
  let dots = range(1, 9).filter(dot => calc.rem(calc.floor(mask / calc.pow(2, dot - 1)), 2) == 1)
  let svg = "<svg xmlns='http://www.w3.org/2000/svg' width='6.4' height='" + str(height) + "' viewBox='0 0 6.4 " + str(height) + "'>" + circles + "</svg>"
  image(bytes(svg), format: "svg", width: 6.4pt, height: height * 1pt,
    alt: "Braille dots " + dots.map(str).join(", "))
}
#let space-key = box(
  inset: (x: 2.4pt, y: 1.5pt), radius: 2pt,
  stroke: 0.4pt + rgb("#aebfc4"), fill: white,
  text(size: 7pt, weight: "bold", fill: muted)[Space],
)
#let sequence(cells, eight: false) = box(
  grid(
    columns: (auto,) * cells.clusters().len(), column-gutter: 3pt,
    align: horizon,
    ..cells.clusters().map(cell => if cell == " " { space-key } else { braille(cell, eight: eight) }),
  )
)
#let output(label, code: false, size: 9.4pt) = text(
  font: if code { mono } else { "Atkinson Hyperlegible" },
  size: size, weight: if code { "medium" } else { "regular" },
  if label == " " { "␣" } else { label },
)

#let pairs(entries, columns: 2, eight: false, code: false, row: 13.8pt, size: 9.4pt) = context {
  let label-width = calc.max(..entries.map(pair => measure(output(pair.at(0), code: code, size: size)).width))
  let count = calc.ceil(entries.len() / columns) * columns
  grid(
    columns: (1fr,) * columns,
    ..range(count).map(i => {
      let row-number = calc.floor(i / columns)
      block(
        width: 100%, height: row,
        inset: (x: 3pt, y: 0pt),
        fill: if calc.even(row-number) { wash } else { white },
        stroke: (bottom: 0.3pt + rule),
      )[
        #if i < entries.len() {
          let pair = entries.at(i)
          grid(
            columns: (label-width, 1fr), column-gutter: 4pt,
            align: (left + horizon, left + horizon),
            output(pair.at(0), code: code, size: size),
            sequence(pair.at(1), eight: eight),
          )
        }
      ]
    }),
  )
}
#let caption(title) = {
  v(3pt)
  heading(level: 2, text(size: 9.2pt, weight: "bold", fill: teal)[#title])
  v(2pt)
}
#let lookup(title, group, mode, columns: 2, code: false, row: 13.8pt, size: 9.4pt) = block(breakable: false)[
  #let entries = find(group, mode)
  #register(group, entries, (mode,))
  #caption(title)
  #pairs(entries, columns: columns, code: code, row: row, size: size)
]
#let page-title(title) = {
  heading(level: 1, text(size: 20pt, weight: "bold", tracking: -0.3pt)[#title])
  v(8pt)
}

#let comparison-rows(group) = {
  let ueb = find(group, "ueb1")
  let eight = find(group, "eight-dot")
  ueb.map(pair => (
    symbol: pair.at(0),
    ueb1: pair.at(1),
    eightdot: eight.find(entry => entry.at(0) == pair.at(0)).at(1),
  ))
}
#let comparison-table(title, entries) = block(breakable: false)[
  #metadata((title: title, capacity: 20, entries: entries)) <reference-comparison>
  #block(height: 14pt)[
    #heading(level: 2, text(size: 7.3pt, weight: "bold", fill: teal)[#title])
  ]
  #table(
    columns: (1fr, 1.15fr, 1fr),
    rows: (17pt,) + (18.5pt,) * 20,
    inset: (x: 2pt, y: 0pt),
    align: center + horizon,
    stroke: 0.3pt + rule,
    fill: (column, row) => if row == 0 { teal } else if calc.odd(row) { wash } else { white },
    table.header(
      ..("Symbol", "UEB 1", "8 dot").map(label => text(size: 6.7pt, weight: "bold", fill: white)[#label]),
    ),
    ..range(20).map(i => if i < entries.len() {
      let entry = entries.at(i)
      (
        output(entry.symbol, code: true, size: 8.7pt),
        sequence(entry.ueb1),
        sequence(entry.eightdot, eight: true),
      )
    } else { ([], [], []) }).flatten(),
  )
]

#let command-labels = (
  "literal-space": "Space", "previous-item-left": "Left", "next-item-right": "Right",
  "up": "Up", "down": "Down", "first-item-home": "Home", "last-item-end": "End",
  "page-up": "Page Up", "page-down": "Page Down", "tab": "Tab", "return": "Return",
  "escape": "Escape", "delete": "Delete", "delete-dot7": "Delete", "delete-space7": "Delete",
  "return-dot8": "Return", "return-space8": "Return", "shift": "Shift", "control": "Control",
  "option": "Option", "command": "Command", "globe": "Globe",
)
#let command-keys(keys) = {
  let numbers = keys.filter(key => type(key) == int)
  let mask = numbers.fold(0, (mask, dot) => mask + calc.pow(2, dot - 1))
  let cells = if keys.contains("space") { (space-key,) } else { () }
  if numbers.len() > 0 {
    if cells.len() > 0 { cells.push(text(size: 8pt, fill: muted)[+]) }
    cells.push(braille(str.from-unicode(0x2800 + mask), eight: true))
  }
  [#metadata((keys: keys, dots: mask)) <reference-chord>]
  box(grid(columns: (auto,) * cells.len(), column-gutter: 3pt, align: horizon, ..cells))
}
#let command-table(index, columns: 4, row: 20pt) = {
  let data = source.commands.at(index)
  [#metadata(data) <reference-commands>]
  let count = calc.ceil(data.commands.len() / columns) * columns
  grid(columns: (1fr,) * columns, ..range(count).map(i => block(
    width: 100%, height: row, inset: (x: 4pt, y: 1pt),
    fill: if calc.even(calc.floor(i / columns)) { wash } else { white },
    stroke: (bottom: 0.3pt + rule, right: if calc.rem(i, columns) != columns - 1 { 0.4pt + rule } else { none }),
  )[
    #if i < data.commands.len() {
      let command = data.commands.at(i)
      grid(columns: (auto, 1fr), column-gutter: 3pt, align: (left + horizon, right + horizon),
        text(size: 9pt)[#command-labels.at(command.name)],
        command-keys(command.inputs.first()),
      )
    }
  ]))
}

#page-title("Characters & keyboard chords")
#let character-groups = (
  "Lowercase letters & Braille Space", "Uppercase letters", "Digits", "Punctuation & symbols",
)
#for group in source.lookups.filter(group => character-groups.contains(group.group)) {
  register(group.group, group.entries, group.modes)
}
#let lower = comparison-rows("Lowercase letters & Braille Space")
#let upper = comparison-rows("Uppercase letters")
#let digits = comparison-rows("Digits")
#let symbols = comparison-rows("Punctuation & symbols")
#grid(columns: (1fr,) * 6, column-gutter: 6pt,
  comparison-table("Lowercase", lower.slice(0, 20)),
  comparison-table("Letters & digits", lower.slice(20, 26) + digits + (lower.last(),)),
  comparison-table("Uppercase", upper.slice(0, 20)),
  comparison-table("Capitals & symbols", upper.slice(20) + symbols.slice(0, 6)),
  comparison-table("Symbols ' – =", symbols.slice(6, 19)),
  comparison-table("Symbols > – ~", symbols.slice(19)),
)
#v(6pt)
#grid(columns: (1fr, 1fr), column-gutter: 13pt,
  lookup("Capitalisation · UEB 1", "Capitalisation", "ueb1", columns: 1, row: 19pt),
  lookup("Numbers & letters · UEB 1", "Numbers & letters", "ueb1", columns: 1, row: 19pt),
)
#v(7pt)
#caption("Factory keys")
#command-table(0)

#pagebreak()
#page-title("Contractions & shortforms · UEB 2")
#grid(columns: (1fr, 1fr, 1fr), column-gutter: 10pt,
  [
    #lookup("Alphabetic wordsigns", "Alphabetic wordsigns · whole words", "ueb2")
    #lookup("Strong wordsigns", "Strong wordsigns · whole words", "ueb2")
    #lookup("Lower wordsigns", "Lower wordsigns · whole words", "ueb2")
    #lookup("Capitalisation", "Capitalisation", "ueb2", columns: 1)
  ],
  [
    #lookup("Strong contractions", "Strong contractions", "ueb2")
    #lookup("Strong groupsigns", "Strong groupsigns · inside words", "ueb2")
    #lookup("Lower groupsigns · word start", "Lower groupsigns · word start", "ueb2")
    #lookup("Lower groupsigns · word middle", "Lower groupsigns · word middle", "ueb2")
    #lookup("Lower groupsigns · en / in", "Lower groupsigns · en / in", "ueb2")
    #lookup("Grade 1 indicators", "Grade 1 indicators · literal letters", "ueb2", columns: 1)
  ],
  [
    #lookup("Initial-letter contractions", "Initial-letter contractions", "ueb2")
    #lookup("Final-letter groupsigns", "Final-letter groupsigns · after a letter", "ueb2")
  ],
)
#v(4pt)
#lookup("Shortforms", "Shortforms · whole words", "ueb2", columns: 5, row: 14.1pt, size: 9.1pt)
#v(7pt)
#caption("Sticky modifiers")
#command-table(1, columns: 5)

// Reference update: character tables compare UEB 1 and 8 dot in six columns,
// with 20 data rows each. Quotation marks use only nondirectional forms;
// command chords, including sticky modifiers, use the same vector cells.
// Only headings and mappings are printed: no running headers, footers,
// page numbers, explanatory prose, or text/code example sections.

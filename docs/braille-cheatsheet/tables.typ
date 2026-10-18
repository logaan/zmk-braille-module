#import "style.typ": teal, rule, wash, table-border, muted
#import "cells.typ": sequence, output, command-keys, chord-cell
#import "mnemonics.typ": mnemonic, mnemonic-spelling

#let mnemonic-headers = ("Output", "Input", "Mnemonic")

// A repeatable table header is also the section heading. Its local counter
// distinguishes the original heading from every column/page continuation.
// Fixed-height heading rows keep the initial and continued layouts identical.
#let section-table(title, group, modes, entries, render,
  columns: 2, headers: (), intro: none, row: 12.5pt, continuation: auto,
  inset: (x: 2pt, y: 0.5pt), label-align: left,
) = {
  let count = columns
  let parts = counter("section-parts:" + group)
  let continued = if continuation == auto { title + " Cont'd" } else { continuation }
  let head = (table.cell(
    colspan: count, fill: white, stroke: none, inset: 0pt, align: left + horizon,
    context {
      parts.step()
      let part = parts.get().first() + 1
      let displayed = if part == 1 { title } else { continued }
      [#metadata((section: title, part: part, heading: displayed)) <reference-fragment>]
      heading(level: 2, text(size: 9.2pt, weight: "bold", fill: teal, displayed))
    },
  ),)
  let heights = (16pt,)
  if intro != none {
    head.push(table.cell(colspan: count, fill: white, stroke: none, inset: (x: 0pt, y: 2pt), intro))
    heights.push(auto)
  }
  if headers.len() > 0 {
    head += headers.map(label => table.cell(
      fill: teal, align: center + horizon,
      text(size: 7.3pt, weight: "bold", fill: white, label),
    ))
    heights.push(13pt)
  }
  let header-rows = heights.len()
  [#metadata((
    title: title, continuation: continued, group: group, modes: modes,
    entries: entries, columns: count, headers: headers,
  )) <reference-section>]
  table(
    columns: (1fr,) * count, rows: heights + (row,) * entries.len() + (0pt,),
    inset: inset,
    align: (x, y) => if x == 0 { label-align + horizon } else { center + horizon },
    stroke: (bottom: 0.3pt + rule),
    fill: (x, y) => if y >= header-rows and calc.even(y - header-rows) { wash } else { white },
    table.header(..head),
    ..entries.enumerate().map(((index, entry)) => {
      let cells = render(entry)
      assert(cells.len() == count)
      let trace = [#metadata((section: title, index: index, entry: entry)) <reference-row>]
      (table.cell(breakable: false, trace + cells.first()),) + cells.slice(1).map(body => table.cell(breakable: false, body))
    }).flatten(),
    // Repeat an explicit rule at every fragment's bottom, including the final
    // fragment. The footer itself is zero-height and contains no printed text.
    table.footer(
      table.hline(stroke: table-border),
      table.cell(colspan: count, inset: 0pt, stroke: none, context [
        #metadata((section: title)) <reference-table-bottom>
      ]),
    ),
  )
  v(3pt)
}
#let character-section(title, group, entries) = section-table(
  title, group, ("ueb1", "eight-dot"), entries,
  entry => (output(entry.at(0), code: true), sequence(entry.at(1)), sequence(entry.at(2), eight: true)),
  columns: 3, headers: ("Symbol", "UEB 1", "8 dot"), row: 15pt, label-align: center,
)
#let word-section(section) = {
  let has-mnemonics = section.entries.any(entry => mnemonic-spelling(entry.at(1)) != none)
  section-table(
    section.title, section.group, ("ueb2",), section.entries,
    entry => (output(entry.at(0)), sequence(entry.at(1), wrap: true))
      + if has-mnemonics { (mnemonic(entry.at(1)),) } else { () },
    columns: if has-mnemonics { 3 } else { 2 },
    headers: if has-mnemonics { mnemonic-headers } else { section.at("headers", default: ()) },
    continuation: section.at("continuation", default: auto), row: auto,
    inset: (x: 2pt, y: 0.2pt),
  )
}
#let chord-section(title, entries, headers: auto) = {
  let has-mnemonics = entries.any(entry => mnemonic-spelling(chord-cell(entry.at(1))) != none)
  section-table(
    title, title, ("ueb1", "eight-dot"), entries,
    entry => (output(entry.at(0)), command-keys(entry.at(1)))
      + if has-mnemonics { (mnemonic(chord-cell(entry.at(1)), space: entry.at(1).contains("space")),) } else { () },
    columns: if has-mnemonics { 3 } else { 2 },
    headers: if headers != auto { headers }
      else if has-mnemonics { mnemonic-headers } else { () }, row: 16pt,
  )
}

// Long passage examples keep their cells at full size: stack only the mode
// label when its sequence would otherwise exceed the page-column width.
#let mode-example(label, cells, available: 100%) = {
  let name = text(size: 7.3pt, fill: muted, label)
  let dots = sequence(cells)
  assert(measure(dots).width <= available, message: "A braille sequence exceeds its page column")
  if measure(name).width + 3pt + measure(dots).width <= available {
    box(grid(columns: (auto, auto), column-gutter: 3pt, align: horizon, name, dots))
  } else {
    name
    v(1pt)
    dots
  }
}
#let capital-example(example) = layout(size => [
  #grid(columns: (1fr, auto), align: horizon, column-gutter: 3pt,
    text(size: 8.2pt, weight: "bold", example.scope), sequence(example.indicator),
  )
  #v(2pt)
  #output(example.example, code: true, size: 8.7pt)
  #v(2pt)
  #mode-example("UEB 1", example.ueb1, available: size.width)
  #v(3pt)
])

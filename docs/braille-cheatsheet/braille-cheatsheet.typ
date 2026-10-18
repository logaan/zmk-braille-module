// Build: typst compile docs/braille-cheatsheet/braille-cheatsheet.typ
// This file owns its data, transformations, templates, and validation.
// Sources: UEB rules, 3rd ed. (2024); liblouis en-us-comp8;
// project mappings and Mac VoiceOver factory chords.

#let ink = rgb("#172e37")
#let muted = rgb("#4f6269")
#let teal = rgb("#006b68")
#let rule = rgb("#d6e1e3")
#let wash = rgb("#f0f5f5")
#let table-border = 1pt + black // Match Typst's default top and side borders.
#let mono = "Atkinson Hyperlegible Mono"
#let page-margins = (left: 11mm, right: 11mm, top: 10mm, bottom: 13mm)

#set document(
  title: "Braille input — A4 quick reference",
  author: "Braille module",
  description: "UEB 1 and 8 dot characters, capitals, keyboard chords, and UEB 2 contractions.",
)
#set page(
  paper: "a4", columns: 3,
  margin: page-margins,
  header: none, footer: none, numbering: none,
)
#set columns(gutter: 8pt)
#set text(
  font: "Atkinson Hyperlegible", size: 9.4pt, fill: ink,
  top-edge: "ascender", bottom-edge: "descender", lang: "en",
)
#set par(leading: 0pt, spacing: 0pt)
#set heading(numbering: none)
#show heading: set block(above: 0pt, below: 0pt)
#show heading.where(level: 1): set text(size: 13.5pt, weight: "bold")

// Character data: (print, UEB 1, 8 dot). Capitals use examples, not an A–Z list.
#let alphabet = "abcdefghijklmnopqrstuvwxyz".clusters().zip(
  "⠁⠃⠉⠙⠑⠋⠛⠓⠊⠚⠅⠇⠍⠝⠕⠏⠟⠗⠎⠞⠥⠧⠺⠭⠽⠵".clusters(),
).map(((letter, cell)) => (letter, cell, cell))
#let lowercase = alphabet + ((" ", " ", " "),)
#let numeral-bases = "⠚⠁⠃⠉⠙⠑⠋⠛⠓⠊".clusters()
#let numeral-eight = "⠴⠂⠆⠒⠲⠢⠖⠶⠦⠔".clusters()
#let digits = range(10).map(i => (str(i), "⠼" + numeral-bases.at(i), numeral-eight.at(i)))
#let symbols = (
  ("!", "⠖", "⠮"),
  ("\"", "⠠⠶", "⠐"),
  ("#", "⠸⠹", "⠼"),
  ("$", "⠈⠎", "⠫"),
  ("%", "⠨⠴", "⠩"),
  ("&", "⠈⠯", "⠯"),
  ("'", "⠄", "⠄"),
  ("(", "⠐⠣", "⠷"),
  (")", "⠐⠜", "⠾"),
  ("*", "⠐⠔", "⠡"),
  ("+", "⠐⠖", "⠬"),
  (",", "⠂", "⠠"),
  ("-", "⠤", "⠤"),
  (".", "⠲", "⠨"),
  ("/", "⠸⠌", "⠌"),
  (":", "⠒", "⠱"),
  (";", "⠆", "⠰"),
  ("<", "⠈⠣", "⠣"),
  ("=", "⠐⠶", "⠿"),
  (">", "⠈⠜", "⠜"),
  ("?", "⠰⠦", "⠹"),
  ("@", "⠈⠁", "⡈"),
  ("[", "⠨⠣", "⡪"),
  ("\\", "⠸⠡", "⡳"),
  ("]", "⠨⠜", "⡻"),
  ("^", "⠈⠢", "⡘"),
  ("_", "⠨⠤", "⠸"),
  ("`", "⠨⠡", "⠈"),
  ("{", "⠸⠣", "⠪"),
  ("|", "⠸⠳", "⠳"),
  ("}", "⠸⠜", "⠻"),
  ("~", "⠈⠔", "⠘"),
)
#let characters = lowercase + digits + symbols
#let capitals = (
  (scope: "Letter prefix", indicator: "⠠", example: "A", ueb1: "⠠⠁"),
  (scope: "Word prefix", indicator: "⠠⠠", example: "ABC", ueb1: "⠠⠠⠁⠃⠉"),
  (scope: "End capitals", indicator: "⠠⠄", example: "ABc", ueb1: "⠠⠠⠁⠃⠠⠄⠉"),
  (scope: "Passage prefix · 3+ words", indicator: "⠠⠠⠠", example: "A BIG CAT", ueb1: "⠠⠠⠠⠁ ⠃⠊⠛ ⠉⠁⠞⠠⠄"),
  (scope: "Grade 1 symbol", indicator: "⠰", example: "b", ueb1: "⠰⠃"),
  (scope: "Grade 1 word", indicator: "⠰⠰", example: "cd", ueb1: "⠰⠰⠉⠙"),
  (scope: "Grade 1 passage", indicator: "⠰⠰⠰", example: "b c d", ueb1: "⠰⠰⠰⠃ ⠉ ⠙⠰⠄"),
)

// Chord data: (host key, simultaneously pressed braille keys).
#let navigation = (
  ("Space", ("space",)),
  ("Left", ("space", 1)),
  ("Right", ("space", 4)),
  ("Up", ("space", 3)),
  ("Down", ("space", 6)),
  ("Home", ("space", 1, 2, 3)),
  ("End", ("space", 4, 5, 6)),
  ("Page Up", ("space", 3, 4, 5, 6)),
  ("Page Down", ("space", 1, 4, 5, 6)),
  ("Tab", ("space", 2, 3, 4, 5)),
  ("Return", ("space", 1, 5)),
  ("Escape", ("space", 1, 2)),
  ("Delete", ("space", 1, 4, 5)),
  ("Delete", (7,)),
  ("Delete", ("space", 7)),
  ("Return", (8,)),
  ("Return", ("space", 8)),
)
#let modifiers = (
  ("Shift", ("space", 4, 7)),
  ("Control", ("space", 3, 7)),
  ("Option", ("space", 2, 7)),
  ("Command", ("space", 1, 7)),
  ("Globe", ("space", 5, 7)),
)
#let dot-mask(keys) = keys.filter(key => type(key) == int).fold(
  0, (mask, dot) => mask + calc.pow(2, dot - 1),
)

// UEB 2 data: each section is one flowing table, never a fixed-size chunk.
#let ueb2 = (
  (title: "Alphabetic wordsigns", group: "Alphabetic wordsigns · whole words", entries: (
    ("as", "⠵"), ("but", "⠃"), ("can", "⠉"), ("do", "⠙"),
    ("every", "⠑"), ("from", "⠋"), ("go", "⠛"), ("have", "⠓"),
    ("it", "⠭"), ("just", "⠚"), ("knowledge", "⠅"), ("like", "⠇"),
    ("more", "⠍"), ("not", "⠝"), ("people", "⠏"), ("quite", "⠟"),
    ("rather", "⠗"), ("so", "⠎"), ("that", "⠞"), ("us", "⠥"),
    ("very", "⠧"), ("will", "⠺"), ("you", "⠽"),
  )),
  (title: "Strong wordsigns", group: "Strong wordsigns · whole words", entries: (
    ("child", "⠡"), ("out", "⠳"), ("shall", "⠩"),
    ("still", "⠌"), ("this", "⠹"), ("which", "⠱"),
  )),
  (title: "Lower wordsigns", group: "Lower wordsigns · whole words", entries: (
    ("be", "⠆"), ("enough", "⠢"), ("his", "⠦"),
    ("in", "⠔"), ("was", "⠴"), ("were", "⠶"),
  )),
  (title: "Strong contractions", group: "Strong contractions", entries: (
    ("and", "⠯"), ("for", "⠿"), ("of", "⠷"), ("the", "⠮"), ("with", "⠾"),
  )),
  (title: "Strong groupsigns", group: "Strong groupsigns · inside words", entries: (
    ("ar", "⠜"), ("ch", "⠡"), ("ed", "⠫"), ("er", "⠻"),
    ("gh", "⠣"), ("ing", "⠬"), ("ou", "⠳"), ("ow", "⠪"),
    ("sh", "⠩"), ("st", "⠌"), ("th", "⠹"), ("wh", "⠱"),
  )),
  (title: "Lower groupsigns · start", group: "Lower groupsigns · word start", entries: (
    ("be", "⠆"), ("con", "⠒"), ("dis", "⠲"),
  )),
  (title: "Lower groupsigns · middle", group: "Lower groupsigns · word middle", entries: (
    ("bb", "⠆"), ("cc", "⠒"), ("ea", "⠂"), ("ff", "⠖"), ("gg", "⠶"),
  )),
  (title: "Lower groupsigns · en / in", group: "Lower groupsigns · en / in", entries: (
    ("en", "⠢"), ("in", "⠔"),
  )),
  (title: "Initial-letter contractions", group: "Initial-letter contractions", entries: (
    ("cannot", "⠸⠉"), ("character", "⠐⠡"), ("day", "⠐⠙"), ("ever", "⠐⠑"),
    ("father", "⠐⠋"), ("had", "⠸⠓"), ("here", "⠐⠓"), ("know", "⠐⠅"),
    ("lord", "⠐⠇"), ("many", "⠸⠍"), ("mother", "⠐⠍"), ("name", "⠐⠝"),
    ("one", "⠐⠕"), ("ought", "⠐⠳"), ("part", "⠐⠏"), ("question", "⠐⠟"),
    ("right", "⠐⠗"), ("some", "⠐⠎"), ("spirit", "⠸⠎"), ("their", "⠸⠮"),
    ("there", "⠐⠮"), ("these", "⠘⠮"), ("those", "⠘⠹"), ("through", "⠐⠹"),
    ("time", "⠐⠞"), ("under", "⠐⠥"), ("upon", "⠘⠥"), ("where", "⠐⠱"),
    ("whose", "⠘⠱"), ("word", "⠘⠺"), ("work", "⠐⠺"), ("world", "⠸⠺"),
    ("young", "⠐⠽"),
  )),
  (title: "Final-letter groupsigns", group: "Final-letter groupsigns · after a letter", entries: (
    ("ance", "⠨⠑"), ("ence", "⠰⠑"), ("ful", "⠰⠇"), ("ity", "⠰⠽"),
    ("less", "⠨⠎"), ("ment", "⠰⠞"), ("ness", "⠰⠎"), ("ong", "⠰⠛"),
    ("ound", "⠨⠙"), ("ount", "⠨⠞"), ("sion", "⠨⠝"), ("tion", "⠰⠝"),
  )),
  (title: "Shortforms", continuation: "ShortForms Cont'd", group: "Shortforms · whole words", entries: (
    ("about", "⠁⠃"), ("above", "⠁⠃⠧"), ("according", "⠁⠉"), ("across", "⠁⠉⠗"),
    ("after", "⠁⠋"), ("afternoon", "⠁⠋⠝"), ("afterward", "⠁⠋⠺"), ("again", "⠁⠛"),
    ("against", "⠁⠛⠌"), ("almost", "⠁⠇⠍"), ("already", "⠁⠇⠗"), ("also", "⠁⠇"),
    ("although", "⠁⠇⠹"), ("altogether", "⠁⠇⠞"), ("always", "⠁⠇⠺"), ("because", "⠆⠉"),
    ("before", "⠆⠋"), ("behind", "⠆⠓"), ("below", "⠆⠇"), ("beneath", "⠆⠝"),
    ("beside", "⠆⠎"), ("between", "⠆⠞"), ("beyond", "⠆⠽"), ("blind", "⠃⠇"),
    ("braille", "⠃⠗⠇"), ("children", "⠡⠝"), ("conceive", "⠒⠉⠧"), ("conceiving", "⠒⠉⠧⠛"),
    ("could", "⠉⠙"), ("deceive", "⠙⠉⠧"), ("deceiving", "⠙⠉⠧⠛"), ("declare", "⠙⠉⠇"),
    ("declaring", "⠙⠉⠇⠛"), ("either", "⠑⠊"), ("first", "⠋⠌"), ("friend", "⠋⠗"),
    ("good", "⠛⠙"), ("great", "⠛⠗⠞"), ("herself", "⠓⠻⠋"), ("him", "⠓⠍"),
    ("himself", "⠓⠍⠋"), ("immediate", "⠊⠍⠍"), ("its", "⠭⠎"), ("itself", "⠭⠋"),
    ("letter", "⠇⠗"), ("little", "⠇⠇"), ("much", "⠍⠡"), ("must", "⠍⠌"),
    ("myself", "⠍⠽⠋"), ("necessary", "⠝⠑⠉"), ("neither", "⠝⠑⠊"), ("oneself", "⠐⠕⠋"),
    ("ourselves", "⠳⠗⠧⠎"), ("paid", "⠏⠙"), ("perceive", "⠏⠻⠉⠧"), ("perceiving", "⠏⠻⠉⠧⠛"),
    ("perhaps", "⠏⠻⠓"), ("quick", "⠟⠅"), ("receive", "⠗⠉⠧"), ("receiving", "⠗⠉⠧⠛"),
    ("rejoice", "⠗⠚⠉"), ("rejoicing", "⠗⠚⠉⠛"), ("said", "⠎⠙"), ("should", "⠩⠙"),
    ("such", "⠎⠡"), ("themselves", "⠮⠍⠧⠎"), ("thyself", "⠹⠽⠋"), ("today", "⠞⠙"),
    ("together", "⠞⠛⠗"), ("tomorrow", "⠞⠍"), ("tonight", "⠞⠝"), ("would", "⠺⠙"),
    ("your", "⠽⠗"), ("yourself", "⠽⠗⠋"), ("yourselves", "⠽⠗⠧⠎"),
  )),
)

// Validate the data at compilation time, without a separate build program.
#assert(alphabet.len() == 26 and digits.len() == 10 and symbols.len() == 32)
#assert(characters.map(entry => entry.at(0)).sorted() == range(32, 127)
  .filter(code => not (65 <= code and code <= 90)).map(str.from-unicode))
#assert(symbols.find(entry => entry.at(0) == "\"").at(1) == "⠠⠶")
#assert(symbols.find(entry => entry.at(0) == "'").at(1) == "⠄")
#assert(ueb2.map(section => section.entries.len()).sum() == 182)
#assert(navigation.len() == 17 and modifiers.len() == 5)
#assert(capitals.len() == 7)
#assert(dot-mask((2, 3, 4, 5)) == 30)

// The same vector cells are used for text, indicators, and command chords.
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
#let sequence(cells, eight: false, wrap: false) = {
  let tokens = cells.clusters().map(cell => if cell == " " { space-key } else { braille(cell, eight: eight) })
  let line(tokens) = box(grid(columns: (auto,) * tokens.len(), column-gutter: 3pt, align: horizon, ..tokens))
  if wrap {
    layout(size => {
      let lines = ()
      let current = ()
      let width = 0pt
      for token in tokens {
        let token-width = measure(token).width
        assert(token-width <= size.width)
        let gap = if current.len() == 0 { 0pt } else { 3pt }
        if width + gap + token-width > size.width {
          lines.push(line(current))
          current = ()
          width = 0pt
          gap = 0pt
        }
        current.push(token)
        width += gap + token-width
      }
      if current.len() > 0 { lines.push(line(current)) }
      grid(columns: (1fr,), row-gutter: 2pt, align: center, ..lines)
    })
  } else {
    line(tokens)
  }
}
#let output(label, code: false, size: 9.4pt) = text(
  font: if code { mono } else { "Atkinson Hyperlegible" }, size: size,
  weight: if code { "medium" } else { "regular" },
  if label == " " { "␣" } else { label },
)
#let command-keys(keys) = {
  let mask = dot-mask(keys)
  let cells = if keys.contains("space") { (space-key,) } else { () }
  if mask > 0 {
    if cells.len() > 0 { cells.push(text(size: 8pt, fill: muted)[+]) }
    cells.push(braille(str.from-unicode(0x2800 + mask), eight: true))
  }
  [#metadata((keys: keys, dots: mask)) <reference-chord>]
  box(grid(columns: (auto,) * cells.len(), column-gutter: 3pt, align: horizon, ..cells))
}

// A repeatable table header is also the section heading. Its local counter
// distinguishes the original heading from every column/page continuation.
// Fixed-height heading rows keep the initial and continued layouts identical.
#let section-table(title, group, modes, entries, render,
  columns: 2, headers: (), intro: none, row: 12.5pt, continuation: auto,
  inset: (x: 2pt, y: 0.5pt),
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
  let heights = (18pt,)
  if intro != none {
    head.push(table.cell(colspan: count, fill: white, stroke: none, inset: (x: 0pt, y: 2pt), intro))
    heights.push(auto)
  }
  if headers.len() > 0 {
    head += headers.map(label => table.cell(
      fill: teal, text(size: 7.3pt, weight: "bold", fill: white, label),
    ))
    heights.push(14pt)
  }
  let header-rows = heights.len()
  [#metadata((title: title, continuation: continued, group: group, modes: modes, entries: entries)) <reference-section>]
  table(
    columns: (1fr,) * count, rows: heights + (row,) * entries.len() + (0pt,),
    inset: inset,
    align: (x, y) => if count == 3 { center + horizon }
      else if x == 0 { left + horizon } else { center + horizon },
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
  v(4pt)
}
#let character-section(title, group, entries) = section-table(
  title, group, ("ueb1", "eight-dot"), entries,
  entry => (output(entry.at(0), code: true), sequence(entry.at(1)), sequence(entry.at(2), eight: true)),
  columns: 3, headers: ("Symbol", "UEB 1", "8 dot"), row: 15pt,
)
#let word-section(section) = section-table(
  section.title, section.group, ("ueb2",), section.entries,
  entry => (output(entry.at(0)), sequence(entry.at(1), wrap: true)),
  continuation: section.at("continuation", default: auto), row: auto,
  inset: (x: 2pt, y: 0.4pt),
)
#let chord-section(title, entries) = section-table(
  title, title, ("ueb1", "eight-dot"), entries,
  entry => (output(entry.at(0)), command-keys(entry.at(1))), row: 16pt,
)

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

// All content follows the page's columns. Sections grow naturally; there are
// no fixed 20-row chunks, side-by-side section grids, or manual column breaks.
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
#chord-section("Sticky modifiers", modifiers)
#heading(level: 1)[Contractions & shortforms · UEB 2]
#v(5pt)
#for section in ueb2 { word-section(section) }

// Layout checks run in Typst too. Every data row must be rendered exactly
// once, and every continued heading must start at the top of its column.
#context {
  let sections = query(<reference-section>)
  let rows = query(<reference-row>)
  if rows.len() > 0 {
    let expected = characters.len() + capitals.len() + navigation.len() + modifiers.len() + ueb2.map(section => section.entries.len()).sum()
    assert(rows.len() == expected, message: "Missing or duplicated reference rows")
    assert(sections.len() == ueb2.len() + 6, message: "Missing or duplicated sections")
    for section in sections {
      let data = section.value
      let actual = rows.filter(row => row.value.section == data.title)
      assert(actual.map(row => row.value.index) == range(data.entries.len()))
    }
    assert(query(<reference-chord>).all(chord => chord.location().page() == 1),
      message: "Keyboard chords and sticky modifiers must stay on page 1")
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

// Reference update: data and transformations live in this Typst template.
// Page-level columns flow every section, repeating “Cont'd” headings wherever
// a section crosses a column or page. Symbols is one continuous section.
// Capitals replace the A–Z list and old Capitalisation section; the mixed
// Numbers & letters section is omitted. All 182 contractions and 22 keyboard
// chords remain, with no running headers, footers, or page numbers.
// Every table uses equal-width tracks and an explicit repeated bottom rule;
// long braille sequences wrap inside their cells without shrinking the dots.
// Bottom borders match the top and sides on every fragment, including column
// continuations, while internal row separators keep their lighter stroke.
// The Capitalised contractions examples are omitted. Grade 1 indicators are
// merged into Capitals, whose examples show only UEB sequences, not 8 dot.
// Braille columns, including wrapped sequences and keyboard chords, are
// centered in every other table; Capitals retains its left-aligned examples.

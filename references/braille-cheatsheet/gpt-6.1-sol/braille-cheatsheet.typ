// A new A4 design. The original cheatsheet supplies information, not layout.
// Build and validate with: python3 references/braille-cheatsheet/gpt-6.1-sol/build.py
#let source = json("source-content.json")
#let ink = rgb("#172e37")
#let muted = rgb("#4f6269")
#let teal = rgb("#006b68")
#let blue = rgb("#285c93")
#let rule = rgb("#d6e1e3")
#let wash = rgb("#f0f5f5")
#let mono = "Atkinson Hyperlegible Mono"

#set document(
  title: "Braille input — two-page A4 quick reference",
  author: "Braille module · gpt-6.1-sol",
  description: "Planned UEB 1 and Computer8 character mappings, all 182 optional UEB 2 contractions, and verified factory chords.",
)
#set page(
  paper: "a4",
  margin: (left: 11mm, right: 11mm, top: 10mm, bottom: 13mm),
  footer-descent: 1.5mm,
  footer: context [
    #line(length: 100%, stroke: 0.45pt + rule)
    #v(3pt)
    #grid(columns: (1fr, auto), align: (left, right),
      text(size: 7.2pt, fill: muted)[BRAILLE MODULE · Planned mappings, not a firmware-support guarantee.],
      text(size: 7.2pt, weight: "bold", fill: teal)[#counter(page).display() / 2],
    )
  ],
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

// Each cell is vector artwork, not a font-dependent stack. Small pale dots
// explicitly mark unpressed positions; active dots retain their normal geometry.
// Alt text spells out the pressed dots in the tagged PDF.
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

// A pair is indivisible. A shared label width aligns the braille beside its
// result, leaving clear white space before the next pair of columns.
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
#let caption(title, note: none, color: teal) = {
  v(3pt)
  grid(columns: (auto, 1fr), column-gutter: 5pt, align: (left + horizon, right + horizon),
    text(size: 9.2pt, weight: "bold", fill: color)[#title],
    if note != none { text(size: 7.1pt, fill: muted)[#note] },
  )
  v(1.5pt)
}
#let lookup(title, group, mode, columns: 2, eight: false, code: false, note: none, row: 13.8pt, size: 9.4pt, color: teal) = block(breakable: false)[
  #let entries = find(group, mode)
  #register(group, entries, (mode,))
  #caption(title, note: note, color: color)
  #pairs(entries, columns: columns, eight: eight, code: code, row: row, size: size)
]
#let page-title(kicker, title, subtitle) = {
  text(size: 7.8pt, tracking: 1.15pt, weight: "bold", fill: teal)[#kicker]
  v(3pt)
  heading(level: 1, text(size: 20pt, weight: "bold", tracking: -0.3pt)[#title])
  v(3pt)
  text(size: 8.5pt, fill: muted)[#subtitle]
}
#let mode-title(title, description, color) = {
  block(width: 100%, inset: (x: 6pt, y: 3pt), fill: color)[
    #heading(level: 2, text(size: 12pt, weight: "bold", fill: white)[#title])
  ]
  v(3pt)
  text(size: 8pt, fill: muted)[#description]
  v(1pt)
}
#let number-guide = box[
  #set text(font: mono, size: 7.5pt, weight: "medium")
  #grid(columns: (auto, auto), column-gutter: 4pt, row-gutter: 0.3pt,
    [1], [4], [2], [5], [3], [6],
    text(fill: blue)[7], text(fill: blue)[8],
  )
]
#let command-labels = (
  "literal-space": "Space", "previous-item-left": "Left", "next-item-right": "Right",
  "up": "Up", "down": "Down", "first-item-home": "Home", "last-item-end": "End",
  "page-up": "Page Up", "page-down": "Page Down", "tab": "Tab", "return": "Return",
  "escape": "Escape", "delete": "Delete", "delete-dot7": "Delete", "delete-space7": "Delete",
  "return-dot8": "Return", "return-space8": "Return", "shift": "Shift", "control": "Control",
  "option": "Option", "command": "Command", "globe": "Globe",
  "volume-down": "Volume −", "volume-up": "Volume +", "play-pause": "Play/pause",
)
#let command-keys(keys) = {
  let numbers = keys.filter(key => type(key) == int)
  let dots = if numbers.len() == 0 { "" } else { numbers.map(str).join() }
  let prefix = if keys.contains("space") { if dots == "" { "Space" } else { "Space + " } } else { "" }
  box(text(size: 8.1pt, weight: "bold", fill: muted, prefix) + text(font: mono, size: 8.4pt, weight: "medium", dots))
}
#let command-table(index, row: 16pt) = {
  let data = source.commands.at(index)
  [#metadata(data) <reference-commands>]
  let count = calc.ceil(data.commands.len() / 4) * 4
  grid(columns: (1fr,) * 4, ..range(count).map(i => block(
    width: 100%, height: row, inset: (x: 4pt, y: 1pt),
    fill: if calc.even(calc.floor(i / 4)) { wash } else { white },
    stroke: (bottom: 0.3pt + rule, right: if calc.rem(i, 4) != 3 { 0.4pt + rule } else { none }),
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

#page-title(
  "BRAILLE INPUT / QUICK REFERENCE",
  "Characters & keyboard chords",
  [UEB 1 · uncontracted literary braille #h(9pt) Computer8 · one cell per character],
)
#v(6pt)
#block(width: 100%, inset: (x: 7pt, y: 5pt), fill: wash, radius: 3pt)[
  #grid(columns: (auto, 100pt, 1fr), column-gutter: 9pt, align: horizon,
    number-guide,
    text(size: 7.8pt, fill: muted)[*Dot positions*\
      UEB text: 1–6\
      Computer8 text: 1–8],
    text(size: 8pt)[*Print → braille.* Read across each table, then down; enter cells *in sequence*.\
      Dark dots = pressed. Faint dots = position guides. #space-key = Braille Space key.],
  )
]
#v(3pt)
#let lower = find("Lowercase letters & Braille Space · UEB 1 / Computer8", "ueb1")
#register("Lowercase letters & Braille Space · UEB 1 / Computer8", lower, ("ueb1", "computer8"))
#caption("Shared lowercase letters", note: [Both modes · space output ␣ = #space-key])
#pairs(lower.slice(0, 26), columns: 13, code: true, row: 15pt)
#v(6pt)

#grid(columns: (1fr, 1fr), column-gutter: 13pt,
  [
    #mode-title("UEB 1", [Capitals and digits include their required prefix cells.], teal)
    #lookup("Uppercase", "Uppercase letters", "ueb1", columns: 6, code: true)
    #lookup("Digits", "Digits", "ueb1", columns: 5, code: true)
    #lookup("Punctuation & symbols", "Punctuation & symbols", "ueb1", columns: 4, code: true)
    #caption("Quotation marks")
    #grid(columns: (1fr, 1fr, 1fr), column-gutter: 5pt,
      [
        #text(size: 7.5pt, weight: "bold", fill: muted)[Opening]
        #v(3pt)
        #let entries = find("Opening", "ueb1")
        #register("Opening", entries, ("ueb1",))
        #pairs(entries, columns: 1, code: true)
      ],
      [
        #text(size: 7.5pt, weight: "bold", fill: muted)[Closing]
        #v(3pt)
        #let entries = find("Closing", "ueb1")
        #register("Closing", entries, ("ueb1",))
        #pairs(entries, columns: 1, code: true)
      ],
      [
        #text(size: 7.5pt, weight: "bold", fill: muted)[Nondirectional]
        #v(3pt)
        #let entries = find("Nondirectional", "ueb1")
        #register("Nondirectional", entries, ("ueb1",))
        #pairs(entries, columns: 1, code: true)
      ],
    )
    #lookup("Capitalisation examples", "Capitalisation", "ueb1", columns: 1)
    #lookup("Numbers & letters", "Numbers & letters", "ueb1", columns: 2)
  ],
  [
    #mode-title("Computer8", [Eight-dot computer braille · liblouis en-us-comp8.], blue)
    #lookup("Uppercase", "Uppercase letters", "computer8", columns: 6, code: true, eight: true, color: blue)
    #lookup("Digits", "Digits", "computer8", columns: 5, code: true, eight: true, color: blue)
    #lookup("Punctuation & symbols", "Punctuation & symbols", "computer8", columns: 4, code: true, eight: true, color: blue)
    #lookup("Text examples", "Text", "computer8", columns: 1, eight: true, color: blue)
    #lookup("Code examples", "Code", "computer8", columns: 1, eight: true, code: true, color: blue)
  ],
)
#v(7pt)
#grid(columns: (auto, 1fr), column-gutter: 8pt, align: (left + horizon, right + horizon),
  heading(level: 2, text(size: 11.5pt, weight: "bold", fill: teal)[Factory keys · both modes]),
  text(size: 7.4pt, fill: muted)[Software verified · hardware acceptance pending],
)
#v(4pt)
#text(size: 8pt)[*Command chords:* press every listed key *together*. Digits are dot numbers, not sequential cells.]
#v(4pt)
#command-table(0)
#v(4pt)
#set par(leading: 1pt)
#text(size: 7.5pt, fill: muted)[
  Tab = Space + dots 2345 (#text(font: mono, size: 7.3pt)[0x11e]). Delete/Return rows are alternative chords. Dot 7/8 commands work in both modes.\
  Sticky modifiers (page 2): next host key, or 1 s unused timeout. Command + Left/Right = line start/end; Shift + Tab = previous UI item.\
  No assigned defaults: Caps Lock, F1–F12, brightness, Mission Control, Spotlight, previous/next track, system mute. Settings entry deferred.
]

#pagebreak()
#set par(leading: 0pt)
#page-title(
  "BRAILLE INPUT / OPTIONAL UEB 2",
  "Contractions & shortforms",
  [All 182 planned contractions · Use the UEB 1 character mappings and indicators on page 1.],
)
#v(6pt)
#block(width: 100%, inset: (x: 7pt, y: 5pt), fill: wash, radius: 3pt)[
  #text(size: 8pt)[*Context matters.* Wordsigns and shortforms below are whole words; groupsigns have the placement shown.\
    Read each table across, then down. Multiple braille cells are a *sequence*, not a simultaneous chord.]
]
#v(5pt)
#grid(columns: (1fr, 1fr, 1fr), column-gutter: 10pt,
  [
    #lookup("Alphabetic wordsigns", "Alphabetic wordsigns · whole words", "ueb2", note: "Whole words")
    #lookup("Strong wordsigns", "Strong wordsigns · whole words", "ueb2", note: "Whole words")
    #lookup("Lower wordsigns", "Lower wordsigns · whole words", "ueb2", note: "Whole words")
    #lookup("Capitalisation", "Capitalisation", "ueb2", columns: 1)
  ],
  [
    #lookup("Strong contractions", "Strong contractions", "ueb2")
    #lookup("Strong groupsigns", "Strong groupsigns · inside words", "ueb2", note: "Inside words")
    #lookup("Lower groupsigns", "Lower groupsigns · word start", "ueb2", note: "Word start")
    #lookup("Lower groupsigns", "Lower groupsigns · word middle", "ueb2", note: "Word middle")
    #lookup("Lower groupsigns", "Lower groupsigns · en / in", "ueb2", note: "en / in")
    #lookup("Grade 1 indicators", "Grade 1 indicators · literal letters", "ueb2", columns: 1, note: "Literal letters")
  ],
  [
    #lookup("Initial-letter contractions", "Initial-letter contractions", "ueb2")
    #lookup("Final-letter groupsigns", "Final-letter groupsigns · after a letter", "ueb2", note: "After a letter")
  ],
)
#v(4pt)
#lookup("Shortforms", "Shortforms · whole words", "ueb2", columns: 5, note: "Whole words · alphabetical order", row: 14.1pt, size: 9.1pt)
#v(7pt)
#grid(columns: (auto, 1fr), column-gutter: 8pt, align: (left + horizon, right + horizon),
  heading(level: 2, text(size: 11.5pt, weight: "bold", fill: teal)[Sticky modifiers & media]),
  text(size: 7.5pt, fill: muted)[Factory chords · UEB 1 / Computer8],
)
#v(4pt)
#command-table(1, row: 17pt)
#v(5pt)
#text(size: 8pt)[*Sticky* = applies to the next host key, or clears after 1 s unused. *Globe ≠ Fn.* Press command chords together.]
#v(6pt)
#text(size: 7.2pt, fill: muted)[
  *Content:* project braille-cheatsheet.typ and its verified research; UEB rules, 3rd ed. (2024); Computer8: liblouis en-us-comp8.\
  *Factory chords:* #link("https://support.apple.com/en-us/103560")[Mac VoiceOver · support.apple.com/en-us/103560].
  Play/pause: #link("https://support.google.com/accessibility/android/answer/3535886")[Android TalkBack · Use braille keystrokes].
]

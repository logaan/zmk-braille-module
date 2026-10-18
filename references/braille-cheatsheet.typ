// Compile from the repository root:
//   typst compile references/braille-cheatsheet.typ
// UEB data comes from the project's verified research, not a second transcription.
// Computer8 follows tests/braille/computer8/characters.yaml (liblouis en-us-comp8).
// Factory commands are sourced Phase-02 bindings.
// Rebuild this PDF before every implementation commit.
#set page(paper: "us-letter", margin: (x: 9mm, y: 8mm))
#set text(
  font: "Avenir", size: 9.5pt, fill: rgb("#17212b"),
  top-edge: "ascender", bottom-edge: "descender",
)
#set par(leading: 0.6em, spacing: 0pt)
#set heading(numbering: none)
#show heading: set block(above: 4pt, below: 3pt)
#show heading.where(level: 1): set text(size: 16pt, weight: "bold", fill: rgb("#126782"))
#show heading.where(level: 2): set text(size: 11pt, weight: "bold", fill: rgb("#126782"))
#show heading.where(level: 3): set text(size: 10pt, weight: "bold")
#show heading.where(level: 4): set text(size: 8.5pt, weight: "bold")

#let pale = rgb("#edf5f7")
#let border-color = rgb("#d7e0e5")

// Keep each sequence unbroken and let its contents determine the row height.
// No fixed-height cells, overlaid labels, negative spacing or font-dependent stacks.
#let sequence(chords) = box(chords.clusters().map(cell => {
  if cell == " " {
    box(
      fill: pale, radius: 1pt, inset: (x: 2pt, y: 1pt),
      text(size: 6.5pt, weight: "bold")[Space],
    )
  } else {
    text(font: "Apple Braille", size: 13.5pt, top-edge: "ascender", bottom-edge: "descender")[#cell]
  }
}).join(h(0.5pt)))

// Every section uses the same result / chord-sequence table, in reading order.
// Metadata makes the displayed mappings available to the reference tests.
#let lookup(title, entries, modes: (), pairs: 3, code: false, level: 3) = block(
  width: 100%, breakable: false, above: 0pt, below: 5pt,
)[
  #metadata((modes: modes, group: title, entries: entries)) <lookup>
  #heading(level: level, title)
  #let cells = entries.map(pair => (
    text(font: if code { "DejaVu Sans Mono" } else { "Avenir" }, weight: "medium")[#if pair.at(0) == " " { [␣] } else { pair.at(0) }],
    sequence(pair.at(1)),
  )).flatten()
  #table(
    columns: range(pairs).map(_ => (auto, 1fr)).flatten(),
    align: left + horizon,
    inset: (x: 3pt, y: 1.5pt),
    stroke: (x: none, y: (paint: border-color, thickness: 0.35pt)),
    fill: (x, y) => if calc.even(x) { pale } else { none },
    ..cells,
  )
]

// Commands use nine-bit simultaneous chords, unlike sequential literary cells.
// Keep separate metadata so command output labels are not treated as text examples.
#let command-data = yaml("research/factory-command-chords.yaml")
#let command-labels = (
  "literal-space": "Space", "previous-item-left": "Left", "next-item-right": "Right",
  "up": "Up", "down": "Down", "first-item-home": "Home", "last-item-end": "End",
  "page-up": "Page Up", "page-down": "Page Down", "tab": "Tab",
  "return": "Return", "escape": "Escape", "delete": "Delete",
  "delete-dot7": "Delete", "delete-space7": "Delete", "return-dot8": "Return",
  "return-space8": "Return", "shift": "Shift", "control": "Control",
  "option": "Option", "command": "Command", "globe": "Globe",
)
#let command-chord(keys) = box({
  let mask = keys.filter(key => type(key) == int).fold(0, (mask, dot) => mask + calc.pow(2, dot - 1))
  if keys.contains("space") {
    text(size: 6.5pt, weight: "bold")[Space]
    if mask != 0 { text(size: 6.5pt, " + ") }
  }
  if mask != 0 { text(font: "Apple Braille", size: 11.5pt)[#str.from-unicode(0x2800 + mask)] }
})
#let command-lookup(title, names) = block(width: 100%, breakable: false, below: 2pt)[
  #let rows = names.map(name => command-data.commands.find(row => row.name == name))
  #metadata((modes: command-data.modes, commands: rows)) <command-lookup>
  #heading(level: 4, title)
  #table(
    columns: (auto, 1fr, auto, 1fr, auto, 1fr, auto, 1fr),
    align: left + horizon, inset: (x: 3pt, y: 1pt),
    stroke: (x: none, y: (paint: border-color, thickness: 0.35pt)),
    fill: (x, y) => if calc.even(x) { pale } else { none },
    ..rows.map(row => (
      text(size: 7.5pt, weight: "medium")[#command-labels.at(row.name)],
      command-chord(row.inputs.first()),
    )).flatten(),
  )
]

#let literary = yaml("research/ueb-literary-signs.yaml").normative
#let alphabet = literary.alphabet.signs.map(sign => (sign.print, sign.braille))
#let capitals = yaml("research/ueb-capitalisation.yaml").normative.capital_letters.signs.map(
  sign => (sign.print, sign.braille),
)
#let digits = yaml("research/ueb-numeric-mode.yaml").normative.numeric_indicators.signs.slice(0, 10).map(
  sign => (sign.print, sign.braille),
).sorted(key: pair => pair.at(0))
// A standalone question mark needs its Grade 1 prefix, unlike the in-word form.
#let symbols = literary.keyboard_coverage.characters.map(sign => (
  sign.print, if sign.print == "?" { "⠰⠦" } else { sign.braille },
)).sorted(key: pair => pair.at(0))
#let quotes = literary.additional_signs.quotation_marks.signs
#let quote-pairs(indices) = indices.map(i => {
  let sign = quotes.at(i)
  (sign.print, sign.braille)
})

#let glyph(mask) = str.from-unicode(0x2800 + mask)
#let computer-capitals = alphabet.map(pair => (
  upper(pair.at(0)), str.from-unicode(pair.at(1).to-unicode() + 64),
))
#let computer-digits = (
  ("0", 52), ("1", 2), ("2", 6), ("3", 18), ("4", 50),
  ("5", 34), ("6", 22), ("7", 54), ("8", 38), ("9", 20),
).map(pair => (pair.at(0), glyph(pair.at(1))))
#let computer-symbols = (
  ("!", 46), ("\"", 16), ("#", 60), ("$", 43), ("%", 41), ("&", 47),
  ("'", 4), ("(", 55), (")", 62), ("*", 33), ("+", 44), (",", 32),
  ("-", 36), (".", 40), ("/", 12), (":", 49), (";", 48), ("<", 35),
  ("=", 63), (">", 28), ("?", 57), ("@", 72), ("[", 106), ("\\", 115),
  ("]", 123), ("^", 88), ("_", 56), ("`", 8), ("{", 42), ("|", 51),
  ("}", 59), ("~", 24),
).map(pair => (pair.at(0), glyph(pair.at(1))))

= Braille · planned character mappings

#lookup("Lowercase letters & Braille Space · UEB 1 / Computer8",
  (..alphabet, (" ", " ")), modes: ("ueb1", "computer8"), pairs: 9, code: true)

#grid(columns: (1fr, 1fr), gutter: 12pt,
  [
    == UEB 1
    #lookup("Uppercase letters", capitals, modes: ("ueb1",), pairs: 4, code: true)
    #lookup("Digits", digits, modes: ("ueb1",), pairs: 5, code: true)
    #lookup("Punctuation & symbols", symbols, modes: ("ueb1",), pairs: 4, code: true)

    #heading(level: 3)[Quotation marks]
    #grid(columns: (1fr, 1fr, 1fr), gutter: 5pt,
      lookup("Opening", quote-pairs((0, 2, 4)), modes: ("ueb1",), pairs: 1, code: true, level: 4),
      lookup("Closing", quote-pairs((1, 3, 5)), modes: ("ueb1",), pairs: 1, code: true, level: 4),
      lookup("Nondirectional", quote-pairs((6, 7)), modes: ("ueb1",), pairs: 1, code: true, level: 4),
    )
    #lookup("Capitalisation", (
      ("ABC", "⠠⠠⠁⠃⠉"),
      ("ABc", "⠠⠠⠁⠃⠠⠄⠉"),
      ("A BIG CAT", "⠠⠠⠠⠁ ⠃⠊⠛ ⠉⠁⠞⠠⠄"),
    ), modes: ("ueb1",), pairs: 1)
    #lookup("Numbers & letters", (
      ("123", "⠼⠁⠃⠉"),
      ("7:30", "⠼⠛⠒⠼⠉⠚"),
      ("3b", "⠼⠉⠰⠃"),
      ("a?", "⠁⠦"),
    ), modes: ("ueb1",), pairs: 2)
  ],
  [
    == Computer8
    #lookup("Uppercase letters", computer-capitals, modes: ("computer8",), pairs: 4, code: true)
    #lookup("Digits", computer-digits, modes: ("computer8",), pairs: 5, code: true)
    #lookup("Punctuation & symbols", computer-symbols, modes: ("computer8",), pairs: 4, code: true)
    #lookup("Text", (
      ("Hello, world!", "⡓⠑⠇⠇⠕⠠ ⠺⠕⠗⠇⠙⠮"),
      ("ABC", "⡁⡃⡉"),
      ("123", "⠂⠆⠒"),
      ("7:30", "⠶⠱⠒⠴"),
      ("3b", "⠒⠃"),
      ("a?", "⠁⠹"),
    ), modes: ("computer8",), pairs: 1)
    #lookup("Code", (
      ("a[i]", "⠁⡪⠊⡻"),
      ("b{j}", "⠃⠪⠚⠻"),
      ("x = 42;", "⠭ ⠿ ⠲⠆⠰"),
      ("\\ | /", "⡳ ⠳ ⠌"),
    ), modes: ("computer8",), pairs: 1)
  ],
)

#command-lookup("Factory keys · UEB 1 / Computer8 · software verified, hardware acceptance pending", (
  "literal-space", "previous-item-left", "next-item-right", "up", "down",
  "first-item-home", "last-item-end", "page-up", "page-down", "tab", "return",
  "escape", "delete", "delete-dot7", "delete-space7", "return-dot8", "return-space8",
))
#block(above: 0pt, below: 0pt)[
  #set text(size: 6.5pt)
  #set par(leading: 1pt)
  Space + cell means one simultaneous chord with Braille Space. Tab = Space + dots 2345 (`0x11e`). Source: Mac VoiceOver, support.apple.com/en-us/103560.
  #linebreak()
  Sticky: next host key or 1 s unused timeout. Command + Left/Right = line start/end; Shift + Tab = previous UI item. Globe ≠ Fn.
  #linebreak()
  Settings entry deferred.
]

#pagebreak()

#let contractions = yaml("research/ueb-grade2-contractions.yaml").normative.classes
#let contraction-pairs(id) = contractions.find(class => class.id == id).entries.map(
  entry => (entry.print, entry.braille),
).sorted(key: pair => pair.at(0))
#let contraction-table(title, id, pairs: 2) = lookup(
  title, contraction-pairs(id), modes: ("ueb2",), pairs: pairs,
)

= UEB 2 · optional planned contractions

#grid(columns: (1fr, 1fr, 1fr), gutter: 10pt,
  [
    #contraction-table("Alphabetic wordsigns · whole words", "alphabetic_wordsigns")
    #contraction-table("Strong wordsigns · whole words", "strong_wordsigns")
    #contraction-table("Lower wordsigns · whole words", "lower_wordsigns")
    #lookup("Capitalisation", (
      ("Will", "⠠⠺"), ("CHILD", "⠠⠠⠡"), ("LITTLE", "⠠⠠⠇⠇"),
    ), modes: ("ueb2",), pairs: 1)
  ],
  [
    #contraction-table("Strong contractions", "strong_contractions")
    #contraction-table("Strong groupsigns · inside words", "strong_groupsigns")
    #let lower-groups = contraction-pairs("lower_groupsigns")
    #lookup("Lower groupsigns · word start", lower-groups.filter(pair =>
      ("be", "con", "dis").contains(pair.at(0))), modes: ("ueb2",), pairs: 2)
    #lookup("Lower groupsigns · word middle", lower-groups.filter(pair =>
      ("ea", "bb", "cc", "ff", "gg").contains(pair.at(0))), modes: ("ueb2",), pairs: 2)
    #lookup("Lower groupsigns · en / in", lower-groups.filter(pair =>
      ("en", "in").contains(pair.at(0))), modes: ("ueb2",), pairs: 2)
    #lookup("Grade 1 indicators · literal letters", (
      ("b", "⠰⠃"),
      ("cd", "⠰⠰⠉⠙"),
      ("b c d", "⠰⠰⠰⠃ ⠉ ⠙⠰⠄"),
    ), modes: ("ueb2",), pairs: 1)
  ],
  [
    #contraction-table("Initial-letter contractions", "initial_letter_contractions")
    #contraction-table("Final-letter groupsigns · after a letter", "final_letter_groupsigns")
  ],
)

#contraction-table("Shortforms · whole words", "shortforms", pairs: 5)

#command-lookup("Factory sticky modifiers · both modes · Mac VoiceOver", (
  "shift", "control", "option", "command", "globe",
))
#text(size: 6.5pt)[Software verification: 22 factory rules in both modes, real ZMK sticky/HID tests, and all 7 keyboard firmware targets pass. Physical macOS/BLE acceptance is pending.]
// Command reference refreshed: 17 direct key chords and 5 sticky modifiers.

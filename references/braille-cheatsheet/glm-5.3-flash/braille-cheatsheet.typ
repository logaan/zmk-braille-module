// Braille keyboard cheat sheet — glm-5.3-flash edition.
// Information content mirrors references/braille-cheatsheet.typ and reads the same
// verified research YAMLs (ICEB Rules of UEB 2024; liblouis en-us-comp8; Mac VoiceOver).
// Design difference: every braille cell is drawn as a vector dot grid, so the sheet
// renders identically everywhere, shows cell numbering, and highlights dots 7/8.
// Compile from the repository root:
//   typst compile references/braille-cheatsheet/glm-5.3-flash/braille-cheatsheet.typ

#let ink = rgb("#1b2530")
#let muted = rgb("#5f7180")
#let faint = rgb("#93a5b1")
#let pale = rgb("#f1f5f8")
#let pale2 = rgb("#e7eef2")
#let edge = rgb("#d8e1e7")
#let teal = rgb("#0d7191")
#let orange = rgb("#bf4b0a")
#let violet = rgb("#6d28d9")
#let rose = rgb("#c11d4e")
#let dot78 = rgb("#e0245e")

#let f-sans = "Avenir Next"
#let f-mono = "DejaVu Sans Mono"

#set page(paper: "a4", margin: (x: 8mm, top: 8mm, bottom: 11mm))
#set text(font: f-sans, size: 8pt, fill: ink, lang: "en")
#set par(leading: 0.55em, spacing: 0pt)
#set page(footer: grid(columns: (1fr, auto), gutter: 4pt,
  align(left + horizon, text(size: 5.4pt, fill: faint)[
    Braille module · data: references/research (verified against ICEB Rules of UEB 3rd ed. 2024)
  ]),
  align(right + horizon, context text(size: 5.4pt, fill: faint)[
    page #str(counter(page).get().first()) / 2
  ]),
))

// ---------------------------------------------------------------- braille dots --
// One braille cell drawn as a vector grid: dots 1-2-3 down the left column,
// 4-5-6 down the right, 7-8 below (rose). No font glyph dependency.
// Typst 0.15 has no bit ops: masks are tested with div-eu/rem instead.
#let dot-geo(size) = (
  d: size * 0.30, // dot diameter
  gx: size * 0.44, // column centre pitch
  gy: size * 0.345, // row centre pitch
)

// Raised dot numbers (1..8) of a mask, e.g. 0x05 -> (1, 3).
#let dots-of-mask(mask) = {
  let out = ()
  for d in range(1, 9) {
    let q = calc.floor(mask / calc.pow(2, d - 1))
    if q - calc.floor(q / 2) * 2 == 1 { out.push(d) }
  }
  out
}
#let DOTS = range(256).map(dots-of-mask)

#let bcell(mask, size: 8pt) = {
  let g = dot-geo(size)
  let rows = if mask >= 0x40 { 4 } else { 3 }
  box(width: g.gx + g.d, height: (rows - 1) * g.gy + g.d, stroke: none, {
    for d in DOTS.at(mask) {
      let c = if d >= 7 { dot78 } else { ink }
      let x = if d == 4 or d == 5 or d == 6 or d == 8 { g.gx } else { 0pt }
      let y = if d == 7 or d == 8 { 3 * g.gy } else if d >= 4 { (d - 4) * g.gy } else { (d - 1) * g.gy }
      place(dx: x, dy: y, circle(radius: g.d / 2, fill: c))
    }
  })
}

// A sequence of cells; if any cell uses dots 7/8 every cell gets 4-row geometry so
// the rows stay aligned. A blank cell inside a sequence is a word-space gap.
#let bseq(masks, size: 8pt) = {
  let eight = masks.any(m => m >= 64)
  let g = dot-geo(size)
  let bh = (if eight { 3 } else { 2 }) * g.gy + g.d
  let out = ()
  let first = true
  for m in masks {
    if not first { out.push(h(1.3pt)) }
    if m == 0 { out.push(box(width: g.gx + g.d, height: bh * 0.7)) } else { out.push(bcell(m, size: size)) }
    first = false
  }
  box(out.join())
}

#let space-chip(size: 8pt) = box(
  fill: pale2, stroke: 0.4pt + edge, radius: 2pt, outset: (y: 0.6pt, x: 1pt),
  text(size: 5.2pt, weight: "semibold", fill: muted, tracking: 0.04em)[SPACE],
)

// Dot numbers of a single-cell sequence, e.g. "145".
#let dot-caption(masks) = {
  if masks.len() == 1 and masks.first() != 0 {
    DOTS.at(masks.first()).map(d => str(d)).join("")
  } else { none }
}

// --------------------------------------------------------------------- helpers --
#let masks-from-dots(cells) = cells.map(cell => {
  // A single-dot cell written as (6) is an int, not an array — normalise it.
  let c = if type(cell) == int { (cell,) } else { cell }
  c.fold(0, (m, d) => m + calc.pow(2, d - 1))
})
#let E(p, cells) = (p, masks-from-dots(cells))

#let band(title, color, note: none) = block(breakable: false, above: 7pt, below: 3pt, width: 100%)[
  #grid(columns: (1fr, auto), gutter: 6pt, align: (left + horizon, right + horizon),
    grid(columns: (auto, auto), gutter: 3.5pt, align: (left, left + horizon),
      box(circle(radius: 2.4pt, fill: color)),
      text(size: 9.5pt, weight: "bold", fill: color, tracking: 0.015em)[#title],
    ),
    if note != none { text(size: 6.2pt, fill: muted)[#note] } else { none },
  )
  #v(-0.6pt)
  #line(length: 100%, stroke: 0.55pt + color.lighten(64%))
]

#let sub(title) = block(breakable: false, above: 4.5pt, below: 1.5pt)[
  #text(size: 7.8pt, weight: "bold", fill: ink)[#title]
]

// The one lookup-table helper every section uses: rows of (print, dots) pairs.
#let entry-table(pairs, per-row: 3, size: 8pt, numbered: false, label-mono: false, label-size: 7.5pt, below: 4pt) = block(
  breakable: false, above: 2pt, below: below, width: 100%,
)[
  #let cells = ()
  #for p in pairs {
    let label = p.at(0)
    let masks = p.at(1)
    cells.push(if label == " " or label == "␣" {
      space-chip(size: size)
    } else {
      text(font: if label-mono { f-mono } else { f-sans }, size: label-size, weight: "medium")[#label]
    })
    let dotc = bseq(masks, size: size)
    let cap = if numbered { dot-caption(masks) } else { none }
    cells.push(if cap == none {
      dotc
    } else {
      stack(spacing: 0.7pt, dotc, text(size: 4.2pt, fill: faint)[#cap])
    })
  }
  #table(
    columns: range(per-row).map(_ => (auto, 1fr)).flatten(),
    align: left + horizon,
    inset: (x: 2.5pt, y: 2pt),
    stroke: (x: none, y: 0.35pt + edge),
    fill: (x, y) => if calc.odd(y) { pale } else { none },
    ..cells,
  )
]

// Factory commands are nine-bit simultaneous chords, not literary cells.
#let cmd-data = yaml("../../research/factory-command-chords.yaml")
#let cmd-labels = (
  "literal-space": "Space", "previous-item-left": "Left", "next-item-right": "Right",
  "up": "Up", "down": "Down", "first-item-home": "Home", "last-item-end": "End",
  "page-up": "Page Up", "page-down": "Page Down", "tab": "Tab",
  "return": "Return", "escape": "Escape", "delete": "Delete",
  "delete-dot7": "Delete", "delete-space7": "Delete", "return-dot8": "Return",
  "return-space8": "Return", "shift": "Shift", "control": "Control",
  "option": "Option", "command": "Command", "globe": "Globe",
  "volume-down": "Volume −", "volume-up": "Volume +", "play-pause": "Play/pause",
)

#let command-chord(row, size: 8pt) = {
  let chord = row.inputs.first()
  let mask = chord.filter(k => type(k) == int).fold(0, (m, d) => m + calc.pow(2, d - 1))
  let parts = ()
  if chord.contains("space") {
    parts.push(space-chip(size: size))
    if mask != 0 {
      parts.push(h(1.8pt))
      parts.push(text(size: 6.2pt, fill: muted)[+])
      parts.push(h(1.8pt))
    }
  }
  if mask != 0 { parts.push(bcell(mask, size: size)) }
  box(parts.join())
}

#let command-table(names, per-row: 4, size: 8pt) = block(breakable: false, above: 2pt, below: 3pt, width: 100%)[
  #let cells = ()
  #for name in names {
    let row = cmd-data.commands.find(r => r.name == name)
    cells.push(text(size: 7.3pt, weight: "medium")[#cmd-labels.at(name)])
    cells.push(command-chord(row, size: size))
  }
  #table(
    columns: range(per-row).map(_ => (auto, 1fr)).flatten(),
    align: left + horizon,
    inset: (x: 2.5pt, y: 1.8pt),
    stroke: (x: none, y: 0.35pt + edge),
    fill: (x, y) => if calc.odd(y) { pale } else { none },
    ..cells,
  )
]

// ------------------------------------------------------------------------ data --
#let lit = yaml("../../research/ueb-literary-signs.yaml").normative
#let capd = yaml("../../research/ueb-capitalisation.yaml").normative
#let numd = yaml("../../research/ueb-numeric-mode.yaml").normative
#let g2 = yaml("../../research/ueb-grade2-contractions.yaml").normative

#let alphabet = lit.alphabet.signs.map(s => E(s.print, s.dots))
#let ueb1-capitals = capd.capital_letters.signs.map(s => E(s.print, s.dots))
#let ueb1-digits = numd.numeric_indicators.signs.slice(0, 10).map(s => E(s.print, s.dots)).sorted(key: p => p.at(0))
#let ueb1-symbols = lit.keyboard_coverage.characters.map(s => E(s.print, s.dots)).sorted(key: p => p.at(0))
#let quotes = lit.additional_signs.quotation_marks.signs
#let qpair(i) = E(quotes.at(i).print, quotes.at(i).dots)
// Unified quotation-mark table: single/double-cell double-quote forms then singles.
#let quotes-table = block(breakable: false, above: 1.5pt, below: 2.5pt, width: 100%)[
  #let qcell(i) = (
    text(font: f-mono, size: 7.3pt, weight: "medium")[#quotes.at(i).print],
    bseq(qpair(i).at(1), size: 8pt),
  )
  #let head(c) = align(center, text(size: 5.8pt, fill: muted, weight: "semibold")[#c])
  #table(
    columns: (auto, 1fr, auto, 1fr, auto, 1fr),
    align: left + horizon,
    inset: (x: 2.5pt, y: 2pt),
    stroke: (x: none, y: 0.35pt + edge),
    fill: (x, y) => if calc.odd(y) { pale } else { none },
    head[opening], [ ], head[closing], [ ], head[nondirectional], [ ],
    ..qcell(0), ..qcell(1), ..qcell(6),
    ..qcell(2), ..qcell(3), ..qcell(7),
    ..qcell(4), ..qcell(5), [ ], [ ],
  )
]

#let cap-examples = (
  E("ABC", ((6), (6), (1), (1, 2), (1, 4))),
  E("ABc", ((6), (6), (1), (1, 2), (6), (3), (1, 4))),
  E("A BIG CAT", ((6), (6), (6), (1), (), (1, 2), (2, 4), (1, 2, 4, 5), (), (1, 4), (1), (2, 3, 4, 5), (6), (3))),
)
#let num-examples = (
  E("123", ((3, 4, 5, 6), (1), (1, 2), (1, 4))),
  E("7:30", ((3, 4, 5, 6), (1, 2, 4, 5), (2, 5), (3, 4, 5, 6), (1, 4), (2, 4, 5))),
  E("3b", ((3, 4, 5, 6), (1, 4), (5, 6), (1, 2))),
  E("a?", ((1), (2, 3, 6))),
)

// Computer8 follows tests/braille/computer8/characters.yaml (liblouis en-us-comp8).
#let c8-capitals = alphabet.slice(0, 26).map(p => (upper(p.at(0)), (p.at(1).first() + 0x40,)))
#let c8-digits = ((0, 52), (1, 2), (2, 6), (3, 18), (4, 50), (5, 34), (6, 22), (7, 54), (8, 38), (9, 20)).map(p => (str(p.at(0)), (p.at(1),)))
#let c8-symbols = (
  ("!", 46), ("\"", 16), ("#", 60), ("$", 43), ("%", 41), ("&", 47),
  ("'", 4), ("(", 55), (")", 62), ("*", 33), ("+", 44), (",", 32),
  ("-", 36), (".", 40), ("/", 12), (":", 49), (";", 48), ("<", 35),
  ("=", 63), (">", 28), ("?", 57), ("@", 72), ("[", 106), ("\\", 115),
  ("]", 123), ("^", 88), ("_", 56), ("`", 8), ("{", 42), ("|", 51),
  ("}", 59), ("~", 24),
).map(p => (p.at(0), (p.at(1),)))
#let c8-text = (
  E("Hello, world!", ((1, 2, 5, 7), (1, 5), (1, 2, 3), (1, 2, 3), (1, 3, 5), (6), (), (2, 4, 5, 6), (1, 3, 5), (1, 2, 3, 5), (1, 2, 3), (1, 4, 5), (2, 3, 4, 6))),
  E("ABC", ((1, 7), (1, 2, 7), (1, 4, 7))),
  E("123", ((2), (2, 3), (2, 5))),
  E("7:30", ((2, 3, 5, 6), (1, 5, 6), (2, 5), (3, 5, 6))),
  E("3b", ((2, 5), (1, 2))),
  E("a?", ((1), (1, 4, 5, 6))),
)
#let c8-code = (
  E("a[i]", ((1), (2, 4, 6, 7), (2, 4), (1, 2, 4, 5, 6, 7))),
  E("b{j}", ((1, 2), (2, 4, 6), (2, 4, 5), (1, 2, 4, 5, 6))),
  E("x = 42;", ((1, 3, 4, 6), (), (1, 2, 3, 4, 5, 6), (), (2, 5, 6), (2, 3), (5, 6))),
  E("\\ | /", ((1, 2, 5, 6, 7), (), (1, 2, 5, 6), (), (3, 4))),
)

#let gclass(id) = g2.classes.find(c => c.id == id)
#let gpairs(id) = gclass(id).entries.map(e => E(e.print, e.dots)).sorted(key: p => p.at(0))
#let lower-groups = gpairs("lower_groupsigns")

// ------------------------------------------------------------- legend diagram --
#let legend-dot(n, colr) = circle(radius: 4.4pt, stroke: 0.55pt + colr, fill: white, text(size: 4.4pt, fill: colr)[#n])
#let legend-cell = grid(columns: (auto, auto), row-gutter: 1.4pt, column-gutter: 1.8pt,
  legend-dot(1, muted), legend-dot(4, muted),
  legend-dot(2, muted), legend-dot(5, muted),
  legend-dot(3, muted), legend-dot(6, muted),
  legend-dot(7, dot78), legend-dot(8, dot78),
)
#let key-swatch(c, t) = grid(columns: (auto, auto), gutter: 2.2pt, align: (horizon, horizon),
  box(circle(radius: 2.2pt, fill: c)),
  text(size: 6.3pt, fill: c, weight: "bold")[#t],
)

// Inline drawn dots for use inside notes (no unicode-braille font dependency).
#let nd(cells, size: 6.5pt) = bseq(masks-from-dots(cells), size: size)

// =================================================================== page one ==
#block(below: 3pt)[
  #grid(columns: (1fr, auto), gutter: 8pt, align: (left + horizon, right + horizon),
    text(size: 15pt, weight: "bold", fill: ink, tracking: -0.01em)[
      Braille · character & chord reference
    ],
    align(right, text(size: 5.8pt, fill: muted)[
      A4 · 2 pages · layout glm-5.3-flash \
      data: verified research YAMLs · UEB 1 / UEB 2 / Computer 8
    ]),
  )
]

#block(below: 4pt, width: 100%, fill: pale, stroke: 0.5pt + edge, radius: 3pt, inset: (x: 6pt, y: 4pt))[
  #grid(columns: (auto, 1fr, auto), gutter: 9pt, align: (left + horizon, left + horizon, right + horizon),
    legend-cell,
    grid(columns: (1fr, 1fr), gutter: 8pt, row-gutter: 1.5pt,
      text(size: 6.3pt, fill: ink)[
        One box = one braille cell = one chord. Sequences read left → right; the SPACE chip is the blank cell (Braille-space key).
      ],
      text(size: 6.3pt, fill: ink)[
        Cell numbering: dots 1·2·3 run down the left column, 4·5·6 down the right. Rose dots 7·8 belong to 8-dot keys only.
      ],
    ),
    grid(columns: (auto, auto), column-gutter: 7pt, row-gutter: 2pt, align: (horizon, horizon),
      key-swatch(teal, "UEB 1"), key-swatch(orange, "Computer 8"),
      key-swatch(violet, "UEB 2 · p. 2"), key-swatch(rose, "factory chords"),
    ),
  )
]

#band("Letters & Braille space — shared by UEB 1 and Computer 8", teal, note: "single cells · dot numbers shown under each cell")
#entry-table(alphabet, per-row: 9, size: 11pt, numbered: true, label-size: 8.5pt)

#grid(columns: (1fr, 1fr), gutter: 9pt,
  [
    #band("UEB 1 · literary (6-dot)", teal, note: "ICEB Rules of UEB 2024")
    #sub("Uppercase letters")
    #entry-table(ueb1-capitals, per-row: 6, size: 8.5pt, label-size: 7.3pt)
    #sub("Digits")
    #entry-table(ueb1-digits, per-row: 5, size: 8.5pt, label-size: 7.3pt)
    #sub("Punctuation & symbols")
    #entry-table(ueb1-symbols, per-row: 5, size: 8.5pt, label-mono: true, label-size: 7.3pt)
    #block(above: 1.5pt, below: 2pt)[
      #text(size: 5.8pt, fill: muted)[
        #nd(((2, 3, 6))) is an opening double quote only at the start of a word, otherwise ?; a standalone ? at word start takes the grade 1 prefix #nd(((5, 6), (2, 3, 6))). " also from #nd(((4, 5), (2, 3, 6))) #nd(((4, 5), (3, 5, 6))) #nd(((6), (2, 3, 5, 6))); ' also from #nd(((6), (2, 3, 6))) #nd(((6), (3, 5, 6))).
      ]
    ]
    #sub("Quotation marks")
    #quotes-table
    #block(above: 0.5pt, below: 2pt)[
      #text(size: 5.8pt, fill: muted)[
        The US keyboard has only straight quotes: every double form emits ", every single form emits ' (the apostrophe).
      ]
    ]
    #sub("Capitalisation")
    #entry-table(cap-examples, per-row: 1, size: 8.5pt)
    #sub("Numbers & letters")
    #entry-table(num-examples, per-row: 2, size: 8.5pt)
  ],
  [
    #band("Computer 8 · 8-dot", orange, note: "capitals = letter cell + dot 7")
    #sub("Uppercase letters")
    #entry-table(c8-capitals, per-row: 6, size: 8.5pt, label-size: 7.3pt)
    #sub("Digits")
    #entry-table(c8-digits, per-row: 5, size: 8.5pt, label-size: 7.3pt)
    #sub("Punctuation & symbols")
    #entry-table(c8-symbols, per-row: 5, size: 8.5pt, label-mono: true, label-size: 7.3pt)
    #sub("Text")
    #entry-table(c8-text, per-row: 1, size: 8.5pt, label-mono: true, label-size: 7.3pt)
    #sub("Code")
    #entry-table(c8-code, per-row: 1, size: 8.5pt, label-mono: true, label-size: 7.3pt)
  ],
)

#band("Factory keys — navigation, editing & 8-dot aliases", rose, note: "software verified, hardware acceptance pending")
#command-table((
  "literal-space", "previous-item-left", "next-item-right", "up", "down",
  "first-item-home", "last-item-end", "page-up", "page-down", "tab", "return",
  "escape", "delete", "delete-dot7", "delete-space7", "return-dot8", "return-space8",
), per-row: 4, size: 9pt)
#block(above: 1.5pt, below: 0pt)[
  #set text(size: 6pt, fill: muted)
  SPACE + cell means one simultaneous chord with Braille Space; Tab = Space + dots 2345 (0x11e). Source: Mac VoiceOver, support.apple.com/en-us/103560. \
  Sticky: next host key or 1 s unused timeout. Command + Left/Right = line start/end; Shift + Tab = previous UI item. Globe ≠ Fn. \
  Skipped defaults: Caps Lock, F1–F12, brightness, Mission Control, Spotlight, previous/next track, system mute — no sourced chord in project references; Settings entry deferred.
]

// =================================================================== page two ==
#pagebreak()

#block(below: 3pt)[
  #grid(columns: (1fr, auto), gutter: 8pt, align: (left + horizon, right + horizon),
    text(size: 15pt, weight: "bold", fill: ink, tracking: -0.01em)[
      UEB 2 · planned contractions (optional mode)
    ],
    align(right, text(size: 5.8pt, fill: muted)[
      reading rules implemented for input; \
      writing rules listed in research only
    ]),
  )
]

#block(above: 1.5pt, below: 5pt, width: 100%)[
  #let leg(cells) = box(bseq(masks-from-dots(cells), size: 7.5pt), inset: (right: 1.5pt))
  #grid(columns: (auto, auto, auto, auto, auto, 1fr), gutter: 7pt, align: left + horizon,
    box[#leg(((5, 6),)) #text(size: 6.8pt)[grade-1 prefix]],
    box[#leg(((6),)) #text(size: 6.8pt)[capital prefix]],
    box[#leg(((3, 4, 5, 6),)) #text(size: 6.8pt)[numeric prefix]],
    box[#leg(((2, 3, 4, 6),)) #text(size: 6.8pt)[“the”]],
    box[#leg(((4, 5, 6), (1, 4))) #text(size: 6.8pt)[“cannot”]],
    align(right, text(size: 6.8pt, fill: muted)[cells in an entry are read one after another — every box is one braille cell]),
  )
]

#grid(columns: (1fr, 1fr, 1fr), gutter: 9pt,
  [
    #band("Alphabetic wordsigns", violet, note: "whole words")
    #entry-table(gpairs("alphabetic_wordsigns"), per-row: 2, size: 9.5pt, label-size: 8pt)
    #sub("Strong wordsigns · whole words")
    #entry-table(gpairs("strong_wordsigns"), per-row: 2, size: 9.5pt, label-size: 8pt)
    #sub("Lower wordsigns · whole words")
    #entry-table(gpairs("lower_wordsigns"), per-row: 2, size: 9.5pt, label-size: 8pt)
    #sub("Capitalisation")
    #entry-table((
      E("Will", ((6), (2, 4, 5, 6))), E("CHILD", ((6), (6), (1, 6))), E("LITTLE", ((6), (6), (1, 2, 3), (1, 2, 3))),
    ), per-row: 1, size: 9.5pt, label-size: 8pt)
  ],
  [
    #band("Strong & lower groupsigns", violet, note: "")
    #sub("Strong contractions")
    #entry-table(gpairs("strong_contractions"), per-row: 2, size: 9.5pt, label-size: 8pt)
    #sub("Strong groupsigns · inside words")
    #entry-table(gpairs("strong_groupsigns"), per-row: 2, size: 9.5pt, label-size: 8pt)
    #sub("Lower groupsigns · word start")
    #entry-table(lower-groups.filter(p => ("be", "con", "dis").contains(p.at(0))), per-row: 2, size: 9.5pt, label-size: 8pt)
    #sub("Lower groupsigns · word middle")
    #entry-table(lower-groups.filter(p => ("ea", "bb", "cc", "ff", "gg").contains(p.at(0))), per-row: 2, size: 9.5pt, label-size: 8pt)
    #sub("Lower groupsigns · en / in")
    #entry-table(lower-groups.filter(p => ("en", "in").contains(p.at(0))), per-row: 2, size: 9.5pt, label-size: 8pt)
    #sub("Grade 1 indicators · literal letters")
    #entry-table((
      E("b", ((5, 6), (1, 2))),
      E("cd", ((5, 6), (5, 6), (1, 4), (1, 4, 5))),
      E("b c d", ((5, 6), (5, 6), (5, 6), (1, 2), (), (1, 4), (), (1, 4, 5), (5, 6), (3))),
    ), per-row: 1, size: 9.5pt, label-size: 8pt)
  ],
  [
    #band("Initial & final-letter signs", violet, note: "")
    #sub("Initial-letter contractions")
    #entry-table(gpairs("initial_letter_contractions"), per-row: 2, size: 9.5pt, label-size: 8pt)
    #sub("Final-letter groupsigns · after a letter")
    #entry-table(gpairs("final_letter_groupsigns"), per-row: 2, size: 9.5pt, label-size: 8pt)
  ],
)

#band("Shortforms · whole words", violet, note: "75 entries")
#entry-table(gpairs("shortforms"), per-row: 6, size: 9pt, label-size: 7.8pt)

#band("Factory sticky modifiers & media", rose, note: "both modes · Mac VoiceOver; Play/pause: TalkBack")
#command-table((
  "shift", "control", "option", "command", "globe", "volume-down", "volume-up", "play-pause",
), per-row: 4, size: 9.5pt)
#block(above: 1.5pt, below: 0pt)[
  #set text(size: 6.8pt, fill: muted)
  Sticky: next host key or 1 s unused timeout (ZMK &sk settings). Common six-dot chords work in both character modes; dots 7/8 Delete/Return aliases are reachable in every mode. Globe ≠ Fn; GLOBE is host-dependent.
]

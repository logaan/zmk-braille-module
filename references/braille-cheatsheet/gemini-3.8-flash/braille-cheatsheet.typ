#set page(
  paper: "a4",
  margin: (x: 8mm, top: 6mm, bottom: 6mm),
  footer: context [
    #grid(
      columns: (1fr, auto),
      text(size: 6pt, fill: rgb("#64748b"))[ZMK Braille Module Reference Cheatsheet · Planned Character Mappings & Direct Factory Chords],
      text(size: 6.2pt, weight: "bold", fill: rgb("#0f3d64"))[Page #counter(page).display("1 of 2", both: true)],
    )
  ]
)
#set text(
  font: "Atkinson Hyperlegible",
  size: 7.2pt,
  fill: rgb("#0f172a"),
  weight: "regular",
  top-edge: "ascender",
  bottom-edge: "descender",
)
#set par(leading: 0.32em, spacing: 0pt)

#let brand-navy = rgb("#0f3d64")
#let brand-blue = rgb("#0284c7")
#let brand-slate = rgb("#475569")
#let bg-card = rgb("#f8fafc")
#let bg-card-header = rgb("#f1f5f9")
#let bg-zebra = rgb("#f8fafc")
#let border-subtle = rgb("#cbd5e1")
#let border-light = rgb("#e2e8f0")
#let dot-active = rgb("#0f172a")
#let dot-inactive = rgb("#cbd5e1")

#let bdot(active, r: 0.95pt) = {
  box(
    width: 2 * r,
    height: 2 * r,
    circle(
      radius: r,
      fill: if active { dot-active } else { dot-inactive },
      stroke: none,
    )
  )
}

#let bcell(char, eight-dot: false, r: 0.95pt, dx: 1.8pt, dy: 1.8pt) = {
  let code = char.to-unicode()
  if code < 0x2800 or code > 0x28FF {
    return char
  }
  let mask = code - 0x2800
  let is-8 = eight-dot or (calc.rem(calc.div-euclid(mask, 64), 2) == 1) or (calc.div-euclid(mask, 128) == 1)
  
  let has(bit) = calc.rem(calc.div-euclid(mask, calc.pow(2, bit)), 2) == 1
  let d1 = has(0)
  let d2 = has(1)
  let d3 = has(2)
  let d4 = has(3)
  let d5 = has(4)
  let d6 = has(5)
  let d7 = has(6)
  let d8 = has(7)

  box(baseline: if is-8 { 24% } else { 18% }, inset: (x: 0.5pt, y: 0.2pt))[
    #grid(
      columns: (2 * r, 2 * r),
      rows: if is-8 { (2 * r, 2 * r, 2 * r, 2 * r) } else { (2 * r, 2 * r, 2 * r) },
      column-gutter: dx,
      row-gutter: dy,
      bdot(d1, r: r), bdot(d4, r: r),
      bdot(d2, r: r), bdot(d5, r: r),
      bdot(d3, r: r), bdot(d6, r: r),
      ..(if is-8 { (bdot(d7, r: r), bdot(d8, r: r)) } else { () }),
    )
  ]
}

#let bseq(seq, eight-dot: false, r: 0.95pt) = {
  box(seq.clusters().map(ch => {
    if ch == " " {
      box(
        fill: rgb("#e2e8f0"),
        radius: 1.5pt,
        inset: (x: 2pt, y: 0.8pt),
        text(font: "Atkinson Hyperlegible", size: 5.5pt, weight: "bold", fill: rgb("#334155"))[Space]
      )
    } else {
      bcell(ch, eight-dot: eight-dot, r: r)
    }
  }).join(h(1pt)))
}

#let card(title: none, badge: none, body, width: 100%, below: 2.8pt) = block(
  width: width,
  stroke: 0.4pt + border-subtle,
  radius: 3pt,
  fill: white,
  clip: true,
  below: below,
  above: 0pt,
)[
  #if title != none [
    #block(
      width: 100%,
      fill: bg-card-header,
      inset: (x: 4.5pt, y: 1.8pt),
      stroke: (bottom: 0.4pt + border-subtle),
    )[
      #grid(
        columns: (1fr, auto),
        align: (left + horizon, right + horizon),
        text(size: 7.2pt, weight: "bold", fill: brand-navy)[#title],
        if badge != none {
          box(
            fill: brand-navy.lighten(85%),
            radius: 2pt,
            inset: (x: 3pt, y: 0.8pt),
            text(size: 5.4pt, weight: "bold", fill: brand-navy)[#badge],
          )
        }
      )
    ]
  ]
  #block(inset: (x: 3pt, y: 2pt))[#body]
]

// Mini lookup table generator
#let table-lookup(entries, pairs: 4, eight-dot: false, mono: false) = {
  let cells = entries.map(pair => (
    text(font: if mono { "Atkinson Hyperlegible Mono" } else { "Atkinson Hyperlegible" }, weight: "bold", size: 7.1pt)[#if pair.at(0) == " " { [␣] } else { pair.at(0) }],
    bseq(pair.at(1), eight-dot: eight-dot),
  )).flatten()
  table(
    columns: range(pairs).map(_ => (auto, 1fr)).flatten(),
    align: (col, row) => if calc.even(col) { right + horizon } else { left + horizon },
    inset: (x: 2.2pt, y: 0.85pt),
    stroke: (x: none, y: (paint: border-light, thickness: 0.3pt)),
    fill: (col, row) => if calc.even(row) { bg-zebra } else { none },
    ..cells,
  )
}

#let command-data = json("data.json")

#let command-labels = (
  "literal-space": "Space", "previous-item-left": "Left", "next-item-right": "Right",
  "up": "Up", "down": "Down", "first-item-home": "Home", "last-item-end": "End",
  "page-up": "Page Up", "page-down": "Page Down", "tab": "Tab",
  "return": "Return", "escape": "Escape", "delete": "Delete",
  "delete-dot7": "Delete (dot 7)", "delete-space7": "Delete (space 7)",
  "return-dot8": "Return (dot 8)", "return-space8": "Return (space 8)",
  "shift": "Shift", "control": "Control", "option": "Option", "command": "Command",
  "globe": "Globe", "volume-down": "Volume −", "volume-up": "Volume +", "play-pause": "Play/pause",
)

#let cmd-chord(keys) = box({
  let mask = keys.filter(key => type(key) == int).fold(0, (mask, dot) => mask + calc.pow(2, dot - 1))
  if keys.contains("space") {
    box(fill: rgb("#e2e8f0"), radius: 1.5pt, inset: (x: 2pt, y: 0.6pt), text(size: 5.4pt, weight: "bold", fill: rgb("#334155"))[Space])
    if mask != 0 {
      h(1.2pt)
      text(size: 6pt, weight: "bold", fill: brand-slate)[+]
      h(1.2pt)
    }
  }
  if mask != 0 {
    bcell(str.from-unicode(0x2800 + mask), eight-dot: true)
  }
})

#let command-table(names, pairs: 6) = {
  let rows = names.map(name => command-data.commands.find(row => row.name == name))
  let cells = rows.map(row => (
    text(font: "Atkinson Hyperlegible", weight: "bold", size: 6.6pt, fill: rgb("#1e293b"))[#command-labels.at(row.name)],
    cmd-chord(row.inputs.first()),
  )).flatten()
  table(
    columns: range(pairs).map(_ => (auto, 1fr)).flatten(),
    align: (col, row) => if calc.even(col) { left + horizon } else { left + horizon },
    inset: (x: 2.5pt, y: 1.1pt),
    stroke: (x: none, y: (paint: border-light, thickness: 0.3pt)),
    fill: (col, row) => if calc.even(row) { bg-zebra } else { none },
    ..cells,
  )
}

#let dot-guide(eight: false) = {
  let r = 2.4pt
  let dnum(n) = box(
    width: 2 * r, height: 2 * r,
    circle(
      radius: r,
      fill: rgb("#f1f5f9"),
      stroke: 0.4pt + brand-navy,
      align(center + horizon)[#text(size: 4.2pt, weight: "bold", fill: brand-navy)[#n]],
    )
  )
  box(
    stroke: 0.5pt + brand-blue.lighten(40%),
    radius: 2.5pt,
    fill: white,
    inset: (x: 3.5pt, y: 2.5pt),
  )[
    #grid(
      columns: (2 * r, 2 * r),
      rows: if eight { (2 * r, 2 * r, 2 * r, 2 * r) } else { (2 * r, 2 * r, 2 * r) },
      column-gutter: 3pt,
      row-gutter: 2.5pt,
      dnum(1), dnum(4),
      dnum(2), dnum(5),
      dnum(3), dnum(6),
      ..(if eight { (dnum(7), dnum(8)) } else { () }),
    )
  ]
}

// -------------------------------------------------------------
// PAGE 1: BASE CHARACTERS, UEB 1, COMPUTER8 & FACTORY KEYS
// -------------------------------------------------------------

#grid(
  columns: (1fr, auto),
  gutter: 10pt,
  align: horizon,
  [
    #text(size: 12.5pt, weight: "bold", fill: brand-navy)[Braille Input Reference Cheatsheet]
    #v(1pt)
    #text(size: 7pt, fill: brand-slate)[Planned Character Mappings & Factory Chords · Unified English Braille (UEB) & Computer8]
  ],
  [
    #box(
      stroke: 0.4pt + border-subtle,
      radius: 3pt,
      fill: bg-card,
      inset: (x: 6pt, y: 3pt),
    )[
      #grid(
        columns: (auto, 8pt, auto),
        align: horizon,
        [
          #align(center)[#text(size: 5.6pt, weight: "bold", fill: brand-navy)[6-Dot (UEB)]]
          #v(1pt)
          #align(center)[#dot-guide(eight: false)]
        ],
        [],
        [
          #align(center)[#text(size: 5.6pt, weight: "bold", fill: brand-navy)[8-Dot (Comp8)]]
          #v(1pt)
          #align(center)[#dot-guide(eight: true)]
        ]
      )
    ]
  ]
)

#v(1.5pt)

#card(title: "Lowercase Letters & Braille Space · Shared Base (UEB 1 & Computer8)", badge: "27 Chords", below: 2.8pt)[
  #let alpha = (..command-data.alphabet, ("␣", " "))
  #grid(
    columns: range(9).map(_ => 1fr),
    row-gutter: 1.5pt,
    column-gutter: 2pt,
    ..alpha.map(pair => align(center)[
      #box(
        width: 100%,
        fill: bg-card,
        radius: 2pt,
        inset: (y: 1.2pt),
        stroke: 0.3pt + border-light,
      )[
        #text(font: "Atkinson Hyperlegible Mono", weight: "bold", size: 7.2pt)[#pair.at(0)]
        #h(2pt)
        #bseq(pair.at(1))
      ]
    ])
  )
]

#grid(
  columns: (1fr, 1fr),
  gutter: 5pt,
  [
    // LEFT COLUMN: UEB 1
    #card(title: [UEB 1 · Uppercase Letters (Prefix Dot 6 #bcell("⠠"))], badge: "2-Cell")[
      #table-lookup(command-data.capitals, pairs: 4, mono: true)
    ]

    #card(title: [UEB 1 · Digits 0–9 (Numeric Prefix #bcell("⠼"))], badge: "Dots 3-4-5-6")[
      #table-lookup(command-data.digits, pairs: 5, mono: true)
    ]

    #card(title: "UEB 1 · Punctuation & Symbols", badge: "ASCII Standard")[
      #table-lookup(command-data.symbols, pairs: 4, mono: true)
    ]

    #card(title: "UEB 1 · Quotation Marks", badge: "Directional")[
      #grid(
        columns: (1fr, 1fr, 1fr),
        gutter: 2pt,
        [
          #align(center)[#text(size: 5.6pt, weight: "bold", fill: brand-slate)[Opening]]
          #table-lookup(command-data.quotes.opening, pairs: 1, mono: true)
        ],
        [
          #align(center)[#text(size: 5.6pt, weight: "bold", fill: brand-slate)[Closing]]
          #table-lookup(command-data.quotes.closing, pairs: 1, mono: true)
        ],
        [
          #align(center)[#text(size: 5.6pt, weight: "bold", fill: brand-slate)[Nondirectional]]
          #table-lookup(command-data.quotes.nondirectional, pairs: 1, mono: true)
        ],
      )
    ]

    #card(title: "UEB 1 · Capitalisation & Syntax Examples", badge: "Grammar")[
      #table-lookup((
        ("ABC", "⠠⠠⠁⠃⠉"),
        ("ABc", "⠠⠠⠁⠃⠠⠄⠉"),
        ("A BIG CAT", "⠠⠠⠠⠁ ⠃⠊⠛ ⠉⠁⠞⠠⠄"),
      ), pairs: 1)
      #v(1.5pt)
      #table-lookup((
        ("123", "⠼⠁⠃⠉"),
        ("7:30", "⠼⠛⠒⠼⠉⠚"),
        ("3b", "⠼⠉⠰⠃"),
        ("a?", "⠁⠦"),
      ), pairs: 2)
    ]
  ],
  [
    // RIGHT COLUMN: COMPUTER8
    #let glyph(mask) = str.from-unicode(0x2800 + mask)
    #let comp-capitals = command-data.alphabet.map(pair => (
      upper(pair.at(0)), str.from-unicode(pair.at(1).to-unicode() + 64),
    ))
    #let comp-digits = (
      ("0", 52), ("1", 2), ("2", 6), ("3", 18), ("4", 50),
      ("5", 34), ("6", 22), ("7", 54), ("8", 38), ("9", 20),
    ).map(pair => (pair.at(0), glyph(pair.at(1))))
    #let comp-symbols = (
      ("!", 46), ("\"", 16), ("#", 60), ("$", 43), ("%", 41), ("&", 47),
      ("'", 4), ("(", 55), (")", 62), ("*", 33), ("+", 44), (",", 32),
      ("-", 36), (".", 40), ("/", 12), (":", 49), (";", 48), ("<", 35),
      ("=", 63), (">", 28), ("?", 57), ("@", 72), ("[", 106), ("\\", 115),
      ("]", 123), ("^", 88), ("_", 56), ("`", 8), ("{", 42), ("|", 51),
      ("}", 59), ("~", 24),
    ).map(pair => (pair.at(0), glyph(pair.at(1))))

    #card(title: "Computer8 · Uppercase Letters (Single Cell)", badge: "Dot 7 Active")[
      #table-lookup(comp-capitals, pairs: 4, eight-dot: true, mono: true)
    ]

    #card(title: "Computer8 · Digits 0–9 (Single Cell)", badge: "Direct Chords")[
      #table-lookup(comp-digits, pairs: 5, eight-dot: true, mono: true)
    ]

    #card(title: "Computer8 · Punctuation & Symbols", badge: "ASCII 8-Dot")[
      #table-lookup(comp-symbols, pairs: 4, eight-dot: true, mono: true)
    ]

    #card(title: "Computer8 · Text & Code Examples", badge: "Direct Mode")[
      #table-lookup((
        ("Hello, world!", "⡓⠑⠇⠇⠕⠠ ⠺⠕⠗⠇⠙⠮"),
        ("ABC", "⡁⡃⡉"),
        ("123", "⠂⠆⠒"),
        ("7:30", "⠶⠱⠒⠴"),
        ("3b", "⠒⠃"),
        ("a?", "⠁⠹"),
      ), pairs: 1, eight-dot: true)
      #v(1.5pt)
      #table-lookup((
        ("a[i]", "⠁⡪⠊⡻"),
        ("b{j}", "⠃⠪⠚⠻"),
        ("x = 42;", "⠭ ⠿ ⠲⠆⠰"),
        ("\\ | /", "⡳ ⠳ ⠌"),
      ), pairs: 1, eight-dot: true)
    ]
  ]
)

#card(title: "Factory Navigation & Editing Keys · Both Modes", badge: "Software Verified · Mac VoiceOver Standard", below: 0pt)[
  #command-table((
    "literal-space", "previous-item-left", "next-item-right", "up", "down",
    "first-item-home", "last-item-end", "page-up", "page-down", "tab", "return",
    "escape", "delete", "delete-dot7", "delete-space7", "return-dot8", "return-space8",
  ), pairs: 6)
  #v(1.5pt)
  #block(
    fill: bg-card,
    inset: (x: 4pt, y: 1.8pt),
    radius: 2pt,
    stroke: 0.3pt + border-light,
  )[
    #text(size: 5.5pt, fill: rgb("#334155"))[
      *Space + cell*: simultaneous chord with Braille Space key. *Tab* = Space + dots 2-3-4-5 (`0x11e`). Source: Mac VoiceOver (support.apple.com/en-us/103560).\
      *Sticky modifiers*: activate for next host key or time out after 1 s unused. Cmd + Left/Right = line start/end; Shift + Tab = previous UI element. Globe ≠ Fn.\
      *Skipped defaults*: Caps Lock, F1–F12, brightness, Mission Control, Spotlight, track, system mute. Sourced chords pending in references.
    ]
  ]
]

#pagebreak()

// -------------------------------------------------------------
// PAGE 2: UEB 2 CONTRACTIONS & MODIFIERS
// -------------------------------------------------------------

#let contraction-class(id) = command-data.contractions.find(c => c.id == id).entries.map(
  entry => (entry.print, entry.braille),
).sorted(key: pair => pair.at(0))

#let ctable(title, id, pairs: 2, badge: none, below: 2.8pt) = card(
  title: title, badge: badge, below: below,
)[
  #table-lookup(contraction-class(id), pairs: pairs)
]

#grid(
  columns: (1fr, auto),
  gutter: 10pt,
  align: horizon,
  [
    #text(size: 12.5pt, weight: "bold", fill: brand-navy)[UEB Grade 2 · Planned Contractions Reference]
    #v(1pt)
    #text(size: 7pt, fill: brand-slate)[182 Normative Contractions, Grade 1 Indicators & Factory Sticky Modifiers]
  ],
  [
    #box(
      stroke: 0.4pt + border-subtle,
      radius: 3pt,
      fill: bg-card,
      inset: (x: 6pt, y: 3pt),
    )[
      #text(size: 5.8pt, weight: "bold", fill: brand-navy)[UEB Grade 2 Contraction Classes]\
      #text(size: 5.2pt, fill: brand-slate)[Wordsigns · Groupsigns · Initial/Final Letter · Shortforms]
    ]
  ]
)

#v(1.5pt)

#grid(
  columns: (1fr, 1fr, 1.05fr),
  gutter: 5pt,
  [
    // COL 1
    #ctable("Alphabetic Wordsigns", "alphabetic_wordsigns", pairs: 2, badge: "Whole Words · 23")
    #ctable("Strong Wordsigns", "strong_wordsigns", pairs: 2, badge: "Whole Words · 6")
    #ctable("Lower Wordsigns", "lower_wordsigns", pairs: 2, badge: "Whole Words · 6")
    
    #card(title: "UEB 2 · Capitalisation", badge: "Wordsigns")[
      #table-lookup((
        ("Will", "⠠⠺"),
        ("CHILD", "⠠⠠⠡"),
        ("LITTLE", "⠠⠠⠇⠇"),
      ), pairs: 1)
    ]
  ],
  [
    // COL 2
    #ctable("Strong Contractions", "strong_contractions", pairs: 2, badge: "Words & Parts · 5")
    #ctable("Strong Groupsigns", "strong_groupsigns", pairs: 2, badge: "Inside Words · 12")
    
    #let lower-groups = contraction-class("lower_groupsigns")
    #card(title: "Lower Groupsigns", badge: "10 signs")[
      #text(size: 5.6pt, weight: "bold", fill: brand-slate)[Word Start (Prefix)]
      #table-lookup(lower-groups.filter(pair => ("be", "con", "dis").contains(pair.at(0))), pairs: 2)
      #v(1.5pt)
      #text(size: 5.6pt, weight: "bold", fill: brand-slate)[Word Middle (Medial)]
      #table-lookup(lower-groups.filter(pair => ("bb", "cc", "ea", "ff", "gg").contains(pair.at(0))), pairs: 2)
      #v(1.5pt)
      #text(size: 5.6pt, weight: "bold", fill: brand-slate)[Anywhere (en / in)]
      #table-lookup(lower-groups.filter(pair => ("en", "in").contains(pair.at(0))), pairs: 2)
    ]
    #card(title: "Grade 1 Indicators · Literal Letters", badge: "Cancel G2")[
      #table-lookup((
        ("b", "⠰⠃"),
        ("cd", "⠰⠰⠉⠙"),
        ("b c d", "⠰⠰⠰⠃ ⠉ ⠙⠰⠄"),
      ), pairs: 1)
    ]
  ],
  [
    // COL 3
    #ctable("Initial-Letter Contractions", "initial_letter_contractions", pairs: 2, badge: "2-Cell · 33")
    #ctable("Final-Letter Groupsigns", "final_letter_groupsigns", pairs: 2, badge: "After Letter · 12")
  ]
)

#card(title: "Shortforms · Whole Words (Specially Abbreviated)", badge: "75 Common Words", below: 2.8pt)[
  #table-lookup(contraction-class("shortforms"), pairs: 5)
]

#card(title: "Factory Sticky Modifiers & Media Chords · Both Modes", badge: "Mac VoiceOver / TalkBack Standard", below: 0pt)[
  #command-table((
    "shift", "control", "option", "command", "globe", "volume-down", "volume-up", "play-pause",
  ), pairs: 4)
  #v(1.5pt)
  #grid(
    columns: (1fr, 1.15fr),
    gutter: 4pt,
    block(
      fill: bg-card,
      inset: (x: 4pt, y: 1.8pt),
      radius: 2pt,
      stroke: 0.3pt + border-light,
      width: 100%,
    )[
      #text(size: 5.4pt, fill: rgb("#334155"))[
        *Sticky modifiers*: activate for next host key or time out after 1 s unused.\
        *Modifier formula*: Space + dot 7 + modifier dot: Cmd (1), Opt (2), Ctrl (3), Shift (4), Globe (5).\
        *Globe*: Consumer GLOBE replaces physical Fn at `Space + dots 5,7` (no Fn+Delete or Fn+F-keys).\
        *Media*: Play/pause: Android TalkBack (`Space + dots 7,8`); Volume: Mac VoiceOver Interaction.
      ]
    ],
    block(
      fill: bg-card,
      inset: (x: 4pt, y: 1.8pt),
      radius: 2pt,
      stroke: 0.3pt + border-light,
      width: 100%,
    )[
      #text(size: 5.4pt, fill: rgb("#334155"))[
        *Common VoiceOver Chording Compositions:*\
        *Line Start*: Cmd + Left Arrow (#cmd-chord(("space", 1, 7)) #text(size: 5pt)[ then ] #cmd-chord(("space", 1)))\
        *Line End*: Cmd + Right Arrow (#cmd-chord(("space", 1, 7)) #text(size: 5pt)[ then ] #cmd-chord(("space", 4)))\
        *Previous UI Item*: Shift + Tab (#cmd-chord(("space", 4, 7)) #text(size: 5pt)[ then ] #cmd-chord(("space", 2, 3, 4, 5)))\
        *Next UI Item*: Tab (#cmd-chord(("space", 2, 3, 4, 5)))
      ]
    ]
  )
]

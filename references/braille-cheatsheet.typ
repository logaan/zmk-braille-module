#set page(
  paper: "us-letter",
  margin: (x: 9mm, y: 8mm),
  fill: rgb("#ffffff"),
)
#set text(font: "Avenir", size: 7.5pt, fill: rgb("#17212b"))
#set par(leading: 0.78em, spacing: 1.5pt, justify: false)
#set table(inset: (x: 2.5pt, y: 1.5pt), stroke: none)

#let ink = rgb("#17212b")
#let muted = rgb("#536477")
#let blue = rgb("#126782")
#let pale = rgb("#edf5f7")
#let pale-blue = rgb("#dceef2")
#let pale-gold = rgb("#fbf3df")
#let border-color = rgb("#d7e0e5")
#let braille(s, size: 10pt, spaced: false) = {
  let body = text(font: "Apple Braille", size: size, fill: ink)[#s]
  if spaced { underline(body) } else { body }
}
#let section(title, kicker: none) = {
  v(2pt)
  text(size: 10pt, weight: "bold", fill: blue)[#title]
  if kicker != none { h(4pt); text(size: 6.1pt, fill: muted)[#kicker] }
  linebreak()
  v(1pt)
}
#let card(body, fill: pale, stroke: border-color, inset: 5pt) = block(
  width: 100%, fill: fill, stroke: (paint: stroke, thickness: 0.55pt), radius: 4pt,
  inset: inset,
)[#body]
#let titlebar(kicker, title, subtitle) = block(
  width: 100%, fill: ink, radius: 6pt, inset: (x: 10pt, y: 7pt),
)[
  #text(size: 6.2pt, weight: "bold", fill: rgb("#8ed5e3"), tracking: 0.7pt)[#kicker]
  #linebreak()
  #text(size: 19pt, weight: "bold", fill: white)[#title]
  #h(8pt)
  #text(size: 7pt, fill: rgb("#d8e3e9"))[#subtitle]
]
#let ruleline = std.line(length: 100%, stroke: (paint: border-color, thickness: 0.5pt))
#let footer(page, label) = {
  v(3pt)
  ruleline
  v(2pt)
  grid(columns: (1fr, auto), text(size: 5.8pt, fill: muted)[#label], text(size: 5.8pt, fill: muted)[#page / 2])
}
#let minihead(text-content) = text(size: 7.2pt, weight: "bold", fill: ink)[#text-content]
#let tiny(text-content) = text(size: 6.2pt, fill: muted)[#text-content]

#titlebar(
  "ZMK BRAILLE INPUT · PLANNED FEATURE REFERENCE",
  "Braille mode",
  "US keyboard coverage · UEB Grade 1 + optional Grade 2 · 8-dot Computer Braille",
)

#v(4pt)
#card(fill: pale-gold, stroke: rgb("#ead9ad"))[
  #grid(columns: (1.5fr, 1fr), gutter: 8pt,
    [
      #minihead[Reading the chord notation]
      #linebreak()
      A Braille cell is one simultaneous chord; adjacent cells are separate chords in order. An underlined cell means hold Braille Space with its dots. The underlined blank cell #braille("⠀", size: 12pt, spaced: true) is Braille Space by itself (a literal host space).
    ],
    [
      #minihead[Physical keys]
      #linebreak()
      Dots 1–3: `F D S` · dots 4–6: `J K L` · dot 7: `A` · dot 8: `;` · Braille Space: the key bound to `&bspc`.
    ],
  )
]

#v(3pt)
#grid(columns: (0.99fr, 1.01fr), gutter: 8pt,
  [
    #section("UEB 1 · alphabet and numbers", kicker: "Six-dot literary cells; ordinary US-layout host keystrokes")
    #card[
      #minihead[Lowercase letters]
      #v(1pt)
      #let alphabet = (
        ("a", "⠁"), ("b", "⠃"), ("c", "⠉"), ("d", "⠙"), ("e", "⠑"), ("f", "⠋"),
        ("g", "⠛"), ("h", "⠓"), ("i", "⠊"), ("j", "⠚"), ("k", "⠅"), ("l", "⠇"),
        ("m", "⠍"), ("n", "⠝"), ("o", "⠕"), ("p", "⠏"), ("q", "⠟"), ("r", "⠗"),
        ("s", "⠎"), ("t", "⠞"), ("u", "⠥"), ("v", "⠧"), ("w", "⠺"), ("x", "⠭"),
        ("y", "⠽"), ("z", "⠵"),
      )
      #let cells = alphabet.map(pair => (text(size: 7pt, weight: "bold")[#pair.at(0)], braille(pair.at(1), size: 11pt))).flatten()
      #table(columns: 4, align: center, inset: (x: 4pt, y: 1pt), stroke: none, ..cells)
    ]

    #v(3pt)
    #card[
      #minihead[Digits]
      #linebreak()
      Start numeric mode with #braille("⠼"); then `a`–`j` cells mean `1`–`0`. A fresh numeric sign is needed after numeric mode ends.
      #v(2pt)
      #let digits = (("1", "⠁"), ("2", "⠃"), ("3", "⠉"), ("4", "⠙"), ("5", "⠑"), ("6", "⠋"), ("7", "⠛"), ("8", "⠓"), ("9", "⠊"), ("0", "⠚"))
      #let dcells = digits.map(pair => (text(size: 6.5pt, weight: "bold")[#pair.at(0)], braille(pair.at(1), size: 10pt))).flatten()
      #table(columns: 10, align: center, inset: (x: 1.6pt, y: 1pt), stroke: none, ..dcells)
    ]

    #v(3pt)
    #card[
      #minihead[Capitalisation and Grade 1 indicators]
      #v(1pt)
      #table(columns: (auto, 1fr), align: (left, left), inset: (x: 3pt, y: 1pt), stroke: none,
        [#braille("⠠")], [Capital next letter],
        [#braille("⠠⠠")], [Capitals for the word],
        [#braille("⠠⠠⠠")], [Capitals across a passage; #braille("⠠⠄") ends it],
        [#braille("⠰")], [Grade 1 symbol; #braille("⠰⠰") word; #braille("⠰⠰⠰") passage],
        [#braille("⠰⠄")], [Grade 1 terminator],
      )
      #tiny[Indicators change reading context; they do not print as characters. Dot 6 is the capital prefix; dot 5-6 is the Grade 1 indicator.]
    ]
  ],

  [
    #section("UEB 1 · punctuation and symbols", kicker: "All printable US keyboard characters have a literary sign")
    #card[
      #minihead[One-cell punctuation]
      #v(1pt)
      #let punct = (
        ((",", "⠂"), (";", "⠆")), ((":", "⠒"), ("!", "⠖")),
        (("apostrophe", "⠄"), ("hyphen -", "⠤")), (("period .", "⠲"), ("question ?", "⠦")),
      )
      #let pcells = punct.map(row => row.map(pair => (text(size: 6.2pt, weight: "medium")[#pair.at(0)], braille(pair.at(1), size: 10pt))).flatten()).flatten()
      #table(columns: 4, align: center, inset: (x: 3pt, y: 1pt), stroke: none, ..pcells)
      #tiny[At a word start #braille("⠦") opens a double quote; elsewhere it is `?`. To force `?` at word start, use #braille("⠰⠦").]
    ]

    #v(3pt)
    #card[
      #minihead[Multi-cell printable signs]
      #v(1pt)
      #let signs = (
        (("` grave", "⠨⠡"), ("~ tilde", "⠈⠔")),
        (("!", "⠖"), ("@", "⠈⠁")),
        (("#", "⠸⠹"), ("$", "⠈⠎")),
        (("%", "⠨⠴"), ("^", "⠈⠢")),
        (("&", "⠈⠯"), ("*", "⠐⠔")),
        (("+", "⠐⠖"), ("=", "⠐⠶")),
        (("<", "⠈⠣"), (">", "⠈⠜")),
        (("/", "⠸⠌"), ("_", "⠨⠤")),
        (("(", "⠐⠣"), (")", "⠐⠜")),
        (("[", "⠨⠣"), ("]", "⠨⠜")),
        (("{", "⠸⠣"), ("}", "⠸⠜")),
        (("\\", "⠸⠡"), ("|", "⠸⠳")),
      )
      #let scells = signs.map(row => row.map(pair => (text(size: 6.6pt, weight: "medium")[#pair.at(0)], braille(pair.at(1), size: 10pt))).flatten()).flatten()
      #table(columns: 4, align: center, inset: (x: 3pt, y: 1.1pt), stroke: none, ..scells)
    ]

    #v(3pt)
    #card[
      #minihead[Quotation marks]
      #v(1pt)
      #table(columns: (auto, 1fr), align: (left, left), inset: (x: 3pt, y: 1pt), stroke: none,
        [Double opening], [#braille("⠦") or #braille("⠘⠦")],
        [Double closing], [#braille("⠴") or #braille("⠘⠴")],
        [Single opening / closing], [#braille("⠠⠦") / #braille("⠠⠴")],
        [Nondirectional quote], [#braille("⠠⠶") double · #braille("⠄") single],
      )
      #tiny[All double forms emit `"`; all single forms emit `'` on a US keyboard.]
    ]
  ],
)

#v(3pt)
#card(fill: pale-blue, stroke: rgb("#b8d9e0"), inset: 7pt)[
  #grid(columns: (1fr, auto), gutter: 10pt,
    [
      #minihead[Example · type “Hello, world!”]
      #linebreak()
      UEB cells are sequential; the underlined blank Braille cell is the simultaneous Braille Space chord between words.
    ],
    [#braille("⠦⠠⠓⠑⠇⠇⠕⠂", size: 13pt) #braille("⠀", size: 13pt, spaced: true) #braille("⠺⠕⠗⠇⠙⠖⠴", size: 13pt)],
  )
]

#footer("PAGE 1", "CHORD = SIMULTANEOUS DOTS · UNDERLINE = ADD BRAILLE SPACE")

#pagebreak()

#titlebar(
  "ZMK BRAILLE INPUT · CHARACTER MODES, COMMANDS & SCOPE",
  "Computer Braille + actions",
  "Dots 1–8 are independent; the ninth chord bit is Braille Space.",
)

#v(3pt)
#section("Computer8 · US English eight-dot table", kicker: "Each cell below shows the Braille pattern and the printable ASCII character it emits; — is reserved. Row/column headers are hexadecimal mask nibbles.")
#card(fill: rgb("#f7f9fa"), stroke: border-color, inset: 3pt)[
  #let glyph-rows = (
    "⠀⠁⠂⠃⠄⠅⠆⠇⠈⠉⠊⠋⠌⠍⠎⠏",
    "⠐⠑⠒⠓⠔⠕⠖⠗⠘⠙⠚⠛⠜⠝⠞⠟",
    "⠠⠡⠢⠣⠤⠥⠦⠧⠨⠩⠪⠫⠬⠭⠮⠯",
    "⠰⠱⠲⠳⠴⠵⠶⠷⠸⠹⠺⠻⠼⠽⠾⠿",
    "⡀⡁⡂⡃⡄⡅⡆⡇⡈⡉⡊⡋⡌⡍⡎⡏",
    "⡐⡑⡒⡓⡔⡕⡖⡗⡘⡙⡚⡛⡜⡝⡞⡟",
    "⡠⡡⡢⡣⡤⡥⡦⡧⡨⡩⡪⡫⡬⡭⡮⡯",
    "⡰⡱⡲⡳⡴⡵⡶⡷⡸⡹⡺⡻⡼⡽⡾⡿",
    "⢀⢁⢂⢃⢄⢅⢆⢇⢈⢉⢊⢋⢌⢍⢎⢏",
    "⢐⢑⢒⢓⢔⢕⢖⢗⢘⢙⢚⢛⢜⢝⢞⢟",
    "⢠⢡⢢⢣⢤⢥⢦⢧⢨⢩⢪⢫⢬⢭⢮⢯",
    "⢰⢱⢲⢳⢴⢵⢶⢷⢸⢹⢺⢻⢼⢽⢾⢿",
    "⣀⣁⣂⣃⣄⣅⣆⣇⣈⣉⣊⣋⣌⣍⣎⣏",
    "⣐⣑⣒⣓⣔⣕⣖⣗⣘⣙⣚⣛⣜⣝⣞⣟",
    "⣠⣡⣢⣣⣤⣥⣦⣧⣨⣩⣪⣫⣬⣭⣮⣯",
    "⣰⣱⣲⣳⣴⣵⣶⣷⣸⣹⣺⣻⣼⣽⣾⣿",
  )
  #let glyphs = glyph-rows.join("").clusters()
  #let alpha-dots = (1, 3, 9, 25, 17, 11, 27, 19, 10, 26, 5, 7, 13, 29, 21, 15, 31, 23, 14, 30, 37, 39, 58, 45, 61, 53)
  #let letters = ("a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z")
  #let comp8 = (
    ..letters.enumerate().map(pair => (alpha-dots.at(pair.at(0)), pair.at(1))),
    ..letters.enumerate().map(pair => (alpha-dots.at(pair.at(0)) + 64, upper(pair.at(1)))),
    (52, "0"), (2, "1"), (6, "2"), (18, "3"), (50, "4"), (34, "5"), (22, "6"), (54, "7"), (38, "8"), (20, "9"),
    (32, ","), (48, ";"), (49, ":"), (40, "."), (46, "!"), (16, "\""), (4, "'"), (55, "("), (62, ")"), (36, "-"), (56, "_"),
    (35, "<"), (63, "="), (28, ">"), (41, "%"), (44, "+"), (24, "~"), (8, "`"), (47, "&"), (43, "$"), (57, "?"), (42, "{"),
    (106, "["), (59, "}"), (123, "]"), (88, "^"), (72, "@"), (60, "#"), (115, "\\"), (51, "|"), (12, "/"), (33, "*"),
  )
  #let hex-digits = "0123456789ABCDEF"
  #let map-cell(mask) = {
    let found = comp8.find(item => item.at(0) == mask)
    let output = if found == none { "—" } else { found.at(1) }
    let tint = if found == none { rgb("#f3f5f6") } else { white }
    box(width: 100%, height: 6mm, fill: tint, stroke: (paint: border-color, thickness: 0.3pt), radius: 1pt, inset: 0.5pt)[
      #align(center + horizon)[#stack(dir: ttb, spacing: -1pt,
        braille(glyphs.at(mask), size: 8.7pt),
        text(size: 5pt, font: "Avenir", fill: if found == none { muted } else { ink })[#output],
      )]
    ]
  }
  #let header = (text(size: 4.7pt, weight: "bold", fill: muted)[HI/LO], ..range(0, 16).map(n => text(size: 5pt, weight: "bold", fill: muted)[#hex-digits.at(n)]))
  #let body = range(0, 16).map(hi => (
    text(size: 5pt, weight: "bold", fill: muted)[#hex-digits.at(hi)],
    ..range(0, 16).map(lo => map-cell(hi * 16 + lo)),
  )).flatten()
  #table(columns: 17, align: center, inset: (x: 0.7pt, y: 0.5pt), stroke: none, ..header, ..body)
]
#v(2pt)
#tiny[Mask bit = dot: bit 0→1, … bit 7→8. The table contains all 256 patterns: 94 printable non-space ASCII characters map to dots; unmarked entries are reserved. Empty pattern #braille("⠀") cannot be pressed as a chord; Braille Space alone emits a literal space. Reserved patterns produce no host output and may be assigned by a user override. Source: liblouis `en-us-comp8.ctb` (US English 8-dot computer Braille), with its `loweredDigits6Dots.uti` and `latinLetterDef8Dots.uti` includes.]

#v(3pt)
#grid(columns: (1fr, 1fr, 1fr), gutter: 7pt,
  [
    #section("Optional UEB 2", kicker: "Full ICEB reading inventory: 182 contractions")
    #card[
      #table(columns: (1fr, auto), align: (left, right), inset: (x: 1pt, y: 0.7pt), stroke: none,
        [Alphabetic wordsigns — standing alone], [23],
        [Strong wordsigns — standing alone], [6],
        [Strong contractions], [5],
        [Strong groupsigns], [12],
        [Lower wordsigns], [6],
        [Lower groupsigns], [10],
        [Initial-letter contractions], [33],
        [Final-letter groupsigns], [12],
        [Shortforms], [75],
      )
      #tiny[Wordsigns and shortforms depend on word boundaries; lower signs and initial/final contractions have positional rules. When a contraction condition fails, the UEB 1 letter/punctuation meaning applies. UEB 2 reads contractions; it does not choose where to write them.]
      #v(2pt)
      #minihead[Shared matching]
      #linebreak()
      Up to 8 chords per rule; longest valid sequence wins. A user override wins a same-length factory tie; up to 256 overrides persist.
    ]
  ],

  [
    #section("Actions and commands", kicker: "Factory command tables are mode-specific; commands can be extended with user rules")
    #card[
      #minihead[Keyboard / navigation actions]
      #linebreak()
      Left/right, Home/End, previous/next UI item; Tab, Return, Delete, Escape, arrows, Caps Lock; sticky Shift, Control, Option, Command; and F1–F12.
      #v(2pt)
      #minihead[Host functions]
      #linebreak()
      Brightness down/up, Mission Control, Spotlight, previous track, play/pause, next track, mute, volume down/up, plus Globe as a host-dependent sticky modifier.
      #v(2pt)
      #minihead[Command-chord conventions]
      #linebreak()
      An underlined dot pattern means hold those Braille keys with Braille Space. That extra chord bit can separate an action from a character; it is distinct from ordinary host Space.
      #v(2pt)
      #minihead[Per-mode command bindings]
      #linebreak()
      Navigation, key, modifier, media/system and settings actions are planned. Their factory chords must be defined separately for each mode and checked against its character table—especially Computer8’s full 256-pattern space.
      #v(2pt)
      #tiny[Apple VoiceOver command assignments vary by display and can be customized in VoiceOver Utility; there is no single universal Apple mapping to copy.]
    ]
  ],

  [
    #section("Mode, input & settings", kicker: "Persistent, on-device configuration")
    #card[
      #minihead[Character modes]
      #linebreak()
      `ueb1` is uncontracted UEB; `computer8` is 8-dot US ASCII; `ueb2` is an optional Grade 2 extension. Other named tables can be added without changing chord capture.
      #v(2pt)
      #minihead[Chords and context]
      #linebreak()
      A chord contains every Braille key held from first press to last release—no timer. Braille Space is bit 9, separate from ordinary Space. Whitespace commits pending text; layer/mode changes and non-Braille input reset context.
      #v(2pt)
      #minihead[Settings & limits]
      #linebreak()
      Display-guided settings capture/edit rules, select and persist a character mode, search compiled ZMK actions, and keep pending input visible. The settings controls stay fixed; user overrides do not change them.
      #v(2pt)
      `&kp` actions send normal host keystrokes, not Unicode Braille. Globe shortcuts are host-dependent; a true Mac Fn key, Touch ID/power, Dictation/Siri and Do Not Disturb are not emulated.
    ]
  ],
)

#v(2pt)
#tiny[UEB source: ICEB, *The Rules of Unified English Braille*, 3rd ed. (2024), with the project’s verified research tables. Apple’s VoiceOver braille commands are assigned per display and may be customized; this guide describes planned module action coverage, not universal Apple default chords. User action overrides and the selected mode persist across reboot; factory rules are compiled.]
#footer("PAGE 2", "CHORD = SIMULTANEOUS DOTS · UNDERLINE = ADD BRAILLE SPACE")

// Sources: UEB rules, 3rd ed. (2024); liblouis en-us-comp8;
// project mappings and Mac VoiceOver factory chords.

// Character data: (print, UEB 1, 8 dot). Capitals use examples, not an A–Z list.
#let alphabet = "abcdefghijklmnopqrstuvwxyz".clusters().zip(
  "⠁⠃⠉⠙⠑⠋⠛⠓⠊⠚⠅⠇⠍⠝⠕⠏⠟⠗⠎⠞⠥⠧⠺⠭⠽⠵".clusters(),
).map(((letter, cell)) => (letter, cell, cell))
#let lowercase = alphabet
#let numeral-bases = "⠚⠁⠃⠉⠙⠑⠋⠛⠓⠊".clusters()
#let numeral-eight = "⠴⠂⠆⠒⠲⠢⠖⠶⠦⠔".clusters()
#let digit-ueb1 = range(10).map(i => (str(i), "⠼" + numeral-bases.at(i)))
#let digit-eight = range(10).map(i => (str(i), numeral-eight.at(i)))
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
  (">", "⠈⠜", "⠜"),
  ("=", "⠐⠶", "⠿"),
  ("?", "⠰⠦", "⠹"),
  ("@", "⠈⠁", "⡈"),
  ("[", "⠨⠣", "⡪"),
  ("]", "⠨⠜", "⡻"),
  ("\\", "⠸⠡", "⡳"),
  ("^", "⠈⠢", "⡘"),
  ("_", "⠨⠤", "⠸"),
  ("`", "⠨⠡", "⠈"),
  ("{", "⠸⠣", "⠪"),
  ("}", "⠸⠜", "⠻"),
  ("|", "⠸⠳", "⠳"),
  ("~", "⠈⠔", "⠘"),
)
#let characters = lowercase + digit-ueb1 + symbols
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
// Sticky modifiers hold their effect for the next chord, marked with "…".
#let chords = navigation + modifiers.map(entry => (entry.at(0) + "…", entry.at(1), true))

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
  (title: "Strong wordsigns", headers: ("Output", "Input"), group: "Strong wordsigns · whole words", entries: (
    ("child", "⠡"), ("out", "⠳"), ("shall", "⠩"),
    ("still", "⠌"), ("this", "⠹"), ("which", "⠱"),
  )),
  (title: "Lower wordsigns", group: "Lower wordsigns · whole words", entries: (
    ("be", "⠆"), ("enough", "⠢"), ("his", "⠦"),
    ("in", "⠔"), ("was", "⠴"), ("were", "⠶"),
  )),
  (title: "Strong contractions", headers: ("Output", "Input"), group: "Strong contractions", entries: (
    ("and", "⠯"), ("for", "⠿"), ("of", "⠷"), ("the", "⠮"), ("with", "⠾"),
  )),
  (title: "Strong groupsigns", headers: ("Output", "Input"), group: "Strong groupsigns · inside words", entries: (
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

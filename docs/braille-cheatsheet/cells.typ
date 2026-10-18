#import "style.typ": ink, muted, mono

#let dot-mask(keys) = keys.filter(key => type(key) == int).fold(
  0, (mask, dot) => mask + calc.pow(2, dot - 1),
)
#let chord-cell(keys) = str.from-unicode(0x2800 + dot-mask(keys))

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
#let key-box(body) = box(
  inset: (x: 2.4pt, y: 1.5pt), radius: 2pt,
  stroke: 0.4pt + rgb("#aebfc4"), fill: white,
  text(size: 7pt, weight: "bold", fill: muted, body),
)
#let space-key = key-box[␣]
#let sequence(cells, eight: false, wrap: false, space: auto) = {
  let space-token = if space == auto { space-key } else { space }
  let tokens = cells.clusters().map(cell => if cell == " " { space-token } else { braille(cell, eight: eight) })
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
#let command-keys(keys, sticky: false) = {
  let mask = dot-mask(keys)
  let cells = if keys.contains("space") { (space-key,) } else { () }
  if mask > 0 {
    if cells.len() > 0 { cells.push(text(size: 8pt, fill: muted)[+]) }
    cells.push(braille(str.from-unicode(0x2800 + mask), eight: true))
  }
  if sticky { cells.push(output("…")) }
  [#metadata((keys: keys, dots: mask, sticky: sticky)) <reference-chord>]
  box(grid(columns: (auto,) * cells.len(), column-gutter: 3pt, align: horizon, ..cells))
}

// Shared palette, typography, and document-level layout.
#let ink = rgb("#172e37")
#let muted = rgb("#4f6269")
#let teal = rgb("#006b68")
#let blue = rgb("#2b5d8f")
#let rule = rgb("#d6e1e3")
#let wash = rgb("#f0f5f5")
#let table-border = 1pt + black // Match Typst's default top and side borders.
#let mono = "Atkinson Hyperlegible Mono"
#let page-margins = (left: 11mm, right: 11mm, top: 10mm, bottom: 13mm)

#let cheatsheet(body) = {
  set document(
    title: "Braille input — A4 quick reference",
    author: "Braille module",
    description: "UEB 1 and 8 dot characters, capitals, keyboard chords, and UEB 2 contractions.",
  )
  set page(
    paper: "a4", columns: 3,
    margin: page-margins,
    header: none, footer: none, numbering: none,
  )
  set columns(gutter: 8pt)
  set text(
    font: "Atkinson Hyperlegible", size: 9.4pt, fill: ink,
    top-edge: "ascender", bottom-edge: "descender", lang: "en",
  )
  set par(leading: 0pt, spacing: 0pt)
  set heading(numbering: none)
  show heading: set block(above: 0pt, below: 0pt)
  show heading.where(level: 1): set text(size: 13.5pt, weight: "bold")
  body
}

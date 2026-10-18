#!/usr/bin/env python3
"""Build and validate the A4 PDF from the adjacent source-content.json.

Requires Python 3.9+, Typst 0.15+, Poppler, and the Atkinson Hyperlegible
and Atkinson Hyperlegible Mono fonts. No Python packages are required.
"""
from __future__ import annotations

import argparse
from collections import Counter
import json
from pathlib import Path
import shutil
import subprocess
import xml.etree.ElementTree as ET

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
DESIGN = HERE / "braille-cheatsheet.typ"
SNAPSHOT = HERE / "source-content.json"
PDF = HERE / "braille-cheatsheet.pdf"
NS = {"x": "http://www.w3.org/1999/xhtml"}


def run(*args: str) -> str:
    return subprocess.check_output(args, cwd=ROOT, text=True)


def query(path: Path, label: str):
    return json.loads(run(
        "typst", "eval", f"query(<{label}>).map(it => it.value)",
        "--in", str(path), "--root", str(ROOT), "--format", "json",
    ))


def mapping_counter(groups):
    return Counter(
        (group["group"], tuple(group["modes"]), label, sequence)
        for group in groups for label, sequence in group["entries"]
    )


def check_content(snapshot):
    expected = mapping_counter(snapshot["lookups"])
    actual = mapping_counter(query(DESIGN, "reference-lookup"))
    if actual != expected:
        raise AssertionError(
            f"Mappings differ from the source. Missing: {expected - actual}; "
            f"extra: {actual - expected}"
        )
    assert query(DESIGN, "reference-commands") == snapshot["commands"], \
        "Factory chords differ from the source"
    comparisons = query(DESIGN, "reference-comparison")
    tables = [table for table in comparisons if table["group"] is None]
    assert len(tables) == 6, f"Expected six character tables, got {len(tables)}"
    assert all(table["capacity"] == 20 and 0 < len(table["entries"]) <= 20
               for table in tables), "Character tables must have 20 data rows"
    characters = [entry for table in tables for entry in table["entries"]]
    assert Counter(entry["symbol"] for entry in characters) == Counter(
        chr(code) for code in range(32, 127)
    ), "Character tables must show each printable ASCII character exactly once"
    for mode, column in (("ueb1", "ueb1"), ("eight-dot", "eightdot")):
        expected_characters = Counter(
            (label, sequence)
            for group in snapshot["lookups"] if mode in group["modes"]
            for label, sequence in group["entries"] if len(label) == 1
        )
        assert Counter((entry["symbol"], entry[column]) for entry in characters) \
            == expected_characters, f"Incorrect {mode} character mappings"
    quotes = {entry["symbol"]: entry for entry in characters
              if entry["symbol"] in ("\"", "'")}
    assert quotes['"']["ueb1"] == "⠠⠶" and quotes["'"]["ueb1"] == "⠄", \
        "Only nondirectional quotation marks should be shown"
    assert not any(group["group"] in (
        "Opening", "Closing", "Nondirectional", "Text", "Code",
    ) for group in snapshot["lookups"]), "Obsolete quote/example sections remain"

    sequences = [table for table in comparisons if table["group"] is not None]
    assert Counter(table["group"] for table in sequences) == Counter((
        "Capitalisation", "Numbers & letters",
    )), "Both sequence sections must compare UEB 1 and 8 dot"
    eightdot = {entry["symbol"]: entry["eightdot"] for entry in characters}
    for table in sequences:
        assert table["capacity"] == len(table["entries"]), \
            f"Incorrect row count for {table['group']}"
        for mode, column in (("ueb1", "ueb1"), ("eight-dot", "eightdot")):
            expected_sequences = Counter(
                (label, sequence)
                for group in snapshot["lookups"]
                if group["group"] == table["group"] and mode in group["modes"]
                for label, sequence in group["entries"]
            )
            assert Counter((entry["symbol"], entry[column])
                           for entry in table["entries"]) == expected_sequences, \
                f"Incorrect {mode} sequences for {table['group']}"
        for entry in table["entries"]:
            assert entry["eightdot"] == "".join(eightdot[c] for c in entry["symbol"]), \
                f"8-dot sequence differs from its character mappings: {entry['symbol']}"

    expected_chords = []
    for group in snapshot["commands"]:
        for command in group["commands"]:
            keys = command["inputs"][0]
            dots = sum(1 << (key - 1) for key in keys if isinstance(key, int))
            assert command["masks"][0] == dots + (256 if "space" in keys else 0), \
                f"Incorrect chord mask for {command['name']}"
            expected_chords.append((tuple(keys), dots))
    assert Counter((tuple(chord["keys"]), chord["dots"])
                   for chord in query(DESIGN, "reference-chord")) \
        == Counter(expected_chords), "Braille chord cells differ from the source"

    contractions = sum(
        len(group["entries"]) for group in snapshot["lookups"]
        if group["modes"] == ["ueb2"] and group["group"] not in (
            "Capitalisation", "Grade 1 indicators · literal letters",
        )
    )
    assert contractions == 182, f"Expected 182 contractions, got {contractions}"
    commands = sum(len(group["commands"]) for group in snapshot["commands"])
    assert commands == 22, f"Expected 22 factory chords, got {commands}"
    return sum(expected.values()), contractions, commands


def check_layout(snapshot):
    layout = json.loads(run(
        "typst", "eval",
        "(tables: query(table).map(it => (columns: it.columns.len(), "
        "rows: it.rows.len(), position: it.location().position())), "
        "chord_pages: query(<reference-chord>).map(it => it.location().page()))",
        "--in", str(DESIGN), "--root", str(ROOT), "--format", "json",
    ))
    assert set(layout["chord_pages"]) == {1}, \
        "Navigation and Editing and Sticky modifiers must both fit on page 1"
    tables = layout["tables"]
    assert len(tables) == 8 and all(table["columns"] == 3 for table in tables), \
        "Expected eight three-column comparison tables"
    assert all(table["rows"] == 21 for table in tables[:6]), \
        "Character tables must each have a header and 20 data rows"
    for table, name in zip(tables[6:], ("Capitalisation", "Numbers & letters")):
        entries = next(group["entries"] for group in snapshot["lookups"]
                       if group["group"] == name and "ueb1" in group["modes"])
        assert table["rows"] == len(entries) + 1, \
            f"Expected a header and one row per sequence in {name}"
    positions = [table["position"] for table in tables]
    assert all(position["page"] == 1 for position in positions), \
        "Comparison tables must all fit on page 1"
    xs = [float(position["x"].removesuffix("pt")) for position in positions]
    ys = [float(position["y"].removesuffix("pt")) for position in positions]
    assert max(ys[:6]) - min(ys[:6]) < 0.1, "Character tables are not side by side"
    steps = [right - left for left, right in zip(xs[:6], xs[1:6])]
    assert min(steps) > 0 and max(steps) - min(steps) < 0.1, \
        "Character tables are not evenly spaced across the page"
    assert abs(xs[6] - xs[0]) < 0.1 and abs(xs[7] - xs[0]) < 0.1 \
        and ys[0] < ys[6] < ys[7], \
        "Sequence tables must be aligned and stacked below the character tables"


def check_pdf():
    xml = run("pdftotext", "-bbox", str(PDF), "-")
    pages = ET.fromstring(xml).findall(".//x:page", NS)
    assert len(pages) == 2, f"Expected two pages, got {len(pages)}"
    mm = 72 / 25.4
    total_words = 0
    for number, page in enumerate(pages, 1):
        width, height = float(page.get("width")), float(page.get("height"))
        assert abs(width - 210 * mm) < 0.1 and abs(height - 297 * mm) < 0.1, \
            f"Page {number} is not A4: {width} × {height} pt"
        words = [
            (word.text, tuple(float(word.get(key)) for key in (
                "xMin", "yMin", "xMax", "yMax",
            )))
            for word in page.findall(".//x:word", NS)
        ]
        assert words, f"Page {number} contains no extractable text"
        total_words += len(words)
        for index, (text, (x1, y1, x2, y2)) in enumerate(words):
            assert x1 >= 11 * mm - 0.3 and x2 <= width - 11 * mm + 0.3, \
                f"Page {number}: {text!r} exceeds the horizontal print margins"
            assert y1 >= 10 * mm - 0.3 and y2 <= height - 13 * mm + 0.3, \
                f"Page {number}: {text!r} exceeds the vertical print margins"
            for other, (ox1, oy1, ox2, oy2) in words[index + 1:]:
                overlap_x = min(x2, ox2) - max(x1, ox1)
                overlap_y = min(y2, oy2) - max(y1, oy1)
                assert not (overlap_x > 0.3 and overlap_y > 0.3), \
                    f"Page {number}: overlapping text {text!r} / {other!r}"
    return total_words


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--check", action="store_true",
        help="Check the source mappings and existing PDF without rewriting either",
    )
    args = parser.parse_args()
    for tool in ("typst", "pdftotext"):
        if not shutil.which(tool):
            parser.error(f"Required executable not found: {tool}")
    fonts = set(run("typst", "fonts").splitlines())
    missing = {"Atkinson Hyperlegible", "Atkinson Hyperlegible Mono"} - fonts
    if missing:
        parser.error("Missing fonts: " + ", ".join(sorted(missing)))

    snapshot = json.loads(SNAPSHOT.read_text(encoding="utf-8"))
    if args.check:
        assert PDF.is_file(), f"PDF not found: {PDF}"
    else:
        subprocess.run([
            "typst", "compile", "--root", str(ROOT), str(DESIGN), str(PDF),
        ], cwd=ROOT, check=True)

    mappings, contractions, commands = check_content(snapshot)
    check_layout(snapshot)
    words = check_pdf()
    print(f"Validated: 2 A4 pages; {mappings} source mappings; "
          f"{contractions} contractions; {commands} factory chords.")
    print("Character layout: six side-by-side tables; three columns; 20 data rows each.")
    print("Sequence layout: Capitalisation and Numbers & letters compare UEB 1 and 8 dot.")
    print(f"Checked {words} text boxes: no overlaps or print-margin overflow.")
    print(PDF.relative_to(ROOT))


if __name__ == "__main__":
    main()

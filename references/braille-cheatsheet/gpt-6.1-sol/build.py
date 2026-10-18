#!/usr/bin/env python3
"""Refresh the information snapshot, build the A4 PDF, and validate it.

Requires Python 3.9+, Typst 0.15+, Poppler, and the Atkinson Hyperlegible
and Atkinson Hyperlegible Mono fonts. No Python packages are required.
"""
from __future__ import annotations

import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import xml.etree.ElementTree as ET

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
ORIGINAL = ROOT / "references/braille-cheatsheet.typ"
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
    contractions = sum(
        len(group["entries"]) for group in snapshot["lookups"]
        if group["modes"] == ["ueb2"] and group["group"] not in (
            "Capitalisation", "Grade 1 indicators · literal letters",
        )
    )
    assert contractions == 182, f"Expected 182 contractions, got {contractions}"
    commands = sum(len(group["commands"]) for group in snapshot["commands"])
    assert commands == 25, f"Expected 25 factory chords, got {commands}"
    return sum(expected.values()), contractions, commands


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
            assert y1 >= 10 * mm - 0.3 and y2 <= height - 5 * mm + 0.3, \
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
        help="Check the existing snapshot/PDF without rewriting either",
    )
    args = parser.parse_args()
    for tool in ("typst", "pdftotext"):
        if not shutil.which(tool):
            parser.error(f"Required executable not found: {tool}")
    fonts = set(run("typst", "fonts").splitlines())
    missing = {"Atkinson Hyperlegible", "Atkinson Hyperlegible Mono"} - fonts
    if missing:
        parser.error("Missing fonts: " + ", ".join(sorted(missing)))

    snapshot = {
        "source": str(ORIGINAL.relative_to(ROOT)),
        "source_sha256": hashlib.sha256(ORIGINAL.read_bytes()).hexdigest(),
        "lookups": query(ORIGINAL, "lookup"),
        "commands": query(ORIGINAL, "command-lookup"),
    }
    if args.check:
        assert json.loads(SNAPSHOT.read_text()) == snapshot, \
            "The information source changed; rebuild and review the PDF"
        assert PDF.is_file(), f"PDF not found: {PDF}"
    else:
        SNAPSHOT.write_text(json.dumps(snapshot, ensure_ascii=False, indent=2) + "\n")
        subprocess.run([
            "typst", "compile", "--root", str(ROOT), str(DESIGN), str(PDF),
        ], cwd=ROOT, check=True)

    mappings, contractions, commands = check_content(snapshot)
    words = check_pdf()
    print(f"Validated: 2 A4 pages; {mappings} source mappings; "
          f"{contractions} contractions; {commands} factory chords.")
    print(f"Checked {words} text boxes: no overlaps or print-margin overflow.")
    print(PDF.relative_to(ROOT))


if __name__ == "__main__":
    main()

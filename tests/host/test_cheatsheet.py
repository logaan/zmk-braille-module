#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Check the printable reference, its mappings, and its rendered layout.

Requires Typst 0.15+, PyYAML, and the reference fonts. Poppler's pdftotext
additionally enables PDF page-boundary and text-overlap checks.
"""
from collections import Counter
import json
from pathlib import Path
import shutil
import string
import subprocess
import tempfile
import unittest
import xml.etree.ElementTree as ET

import yaml

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "references/braille-cheatsheet.typ"


def load_yaml(path):
    return yaml.safe_load((ROOT / path).read_text())


def braille(inputs):
    return "".join(chr(0x2800 + sum(1 << (dot - 1) for dot in cell))
                   if cell else " " for cell in inputs)


@unittest.skipUnless(shutil.which("typst"), "Typst is not installed")
class CheatsheetReference(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fonts = subprocess.check_output(["typst", "fonts"], text=True).splitlines()
        missing = {"Avenir", "Apple Braille", "DejaVu Sans Mono"} - set(fonts)
        if missing:
            raise unittest.SkipTest(f"Missing reference fonts: {', '.join(sorted(missing))}")
        temporary = tempfile.TemporaryDirectory()
        cls.addClassCleanup(temporary.cleanup)
        cls.pdf = Path(temporary.name) / "cheatsheet.pdf"
        subprocess.run(["typst", "compile", str(SOURCE), str(cls.pdf)], check=True)
        cls.lookups = json.loads(subprocess.check_output([
            "typst", "eval", "query(<lookup>).map(it => it.value)",
            "--in", str(SOURCE), "--format", "json",
        ], text=True))
        cls.command_tables = json.loads(subprocess.check_output([
            "typst", "eval", "query(<command-lookup>).map(it => it.value)",
            "--in", str(SOURCE), "--format", "json",
        ], text=True))

    def entries(self, mode, group=None):
        return [tuple(pair) for table in self.lookups
                if mode in table["modes"] and (group is None or table["group"] == group)
                for pair in table["entries"]]

    def test_all_printable_ascii_results_are_discoverable(self):
        expected = {chr(code) for code in range(32, 127)}
        for mode in ("ueb1", "computer8"):
            with self.subTest(mode=mode):
                actual = {result for result, _ in self.entries(mode) if len(result) == 1}
                self.assertEqual(actual, expected)
        shared = self.lookups[0]
        self.assertEqual(shared["modes"], ["ueb1", "computer8"])
        self.assertEqual([pair[0] for pair in shared["entries"]], list(string.ascii_lowercase + " "))

    def test_ueb1_printable_mappings_match_fixtures(self):
        fixtures = {}
        for name in ("letters", "capitals", "numbers", "punctuation", "indicators"):
            for case in load_yaml(f"tests/braille/ueb1/{name}.yaml")["cases"]:
                fixtures[braille(case["inputs"])] = "".join(case["outputs"])
        for result, chords in self.entries("ueb1"):
            if len(result) != 1:
                continue
            with self.subTest(result=result, chords=chords):
                # Core punctuation is tested after 'a'; an opening quote is
                # tested before 'a'. All other entries can stand on their own.
                candidates = ((chords, result), ("⠁" + chords, "a" + result),
                              (chords + "⠁", result + "a"))
                self.assertTrue(any(fixtures.get(sequence) == output
                                    for sequence, output in candidates))
        for group, prefix, count in (("Uppercase letters", "⠠", 26), ("Digits", "⠼", 10)):
            entries = self.entries("ueb1", group)
            self.assertEqual(len(entries), count)
            self.assertTrue(all(chords.startswith(prefix) for _, chords in entries))
        self.assertIn(("?", "⠰⠦"), self.entries("ueb1", "Punctuation & symbols"))
        self.assertIn(("a?", "⠁⠦"), self.entries("ueb1", "Numbers & letters"))

    def test_computer8_characters_and_examples_match_fixtures(self):
        cases = load_yaml("tests/braille/computer8/characters.yaml")["cases"]
        by_chord = {braille(case["inputs"]): case["outputs"][0] for case in cases
                    if len(case["inputs"]) == 1 and len(case["outputs"]) == 1}
        self.assertEqual(len(by_chord), 95)
        for result, chords in self.entries("computer8"):
            with self.subTest(result=result):
                self.assertEqual("".join(by_chord[cell] for cell in chords), result)

    def test_all_182_contractions_match_verified_research(self):
        research = load_yaml("references/research/ueb-grade2-contractions.yaml")["normative"]
        expected = Counter((entry["print"], braille(entry["dots"]))
                           for group in research["classes"] for entry in group["entries"])
        actual = Counter(tuple(pair) for table in self.lookups if "ueb2" in table["modes"]
                         and table["group"] not in ("Capitalisation", "Grade 1 indicators · literal letters")
                         for pair in table["entries"])
        self.assertEqual(sum(actual.values()), 182)
        self.assertEqual(actual, expected)

    def test_all_verified_factory_commands_are_displayed(self):
        research = load_yaml("references/research/factory-command-chords.yaml")
        actual = [row for table in self.command_tables for row in table["commands"]]
        self.assertEqual(len(actual), 25)
        self.assertEqual(Counter(row["name"] for row in actual),
                         Counter(row["name"] for row in research["commands"]))
        self.assertEqual({row["name"]: row for row in actual},
                         {row["name"]: row for row in research["commands"]})
        for table in self.command_tables:
            self.assertEqual(table["modes"], ["ueb1", "computer8"])
        tab = next(row for row in actual if row["name"] == "tab")
        self.assertEqual(tab["inputs"], [["space", 2, 3, 4, 5]])
        self.assertEqual(tab["masks"], [0x11e])
        self.assertEqual(tab["binding"], "&kp TAB")

    @unittest.skipUnless(shutil.which("pdftotext"), "Poppler's pdftotext is not installed")
    def test_two_pages_without_overlapping_or_out_of_bounds_text(self):
        xml = subprocess.check_output(["pdftotext", "-bbox", str(self.pdf), "-"], text=True)
        ns = {"x": "http://www.w3.org/1999/xhtml"}
        pages = ET.fromstring(xml).findall(".//x:page", ns)
        self.assertEqual(len(pages), 2)
        for number, page in enumerate(pages, 1):
            width, height = float(page.get("width")), float(page.get("height"))
            words = [(word.text, tuple(float(word.get(key)) for key in
                                      ("xMin", "yMin", "xMax", "yMax")))
                     for word in page.findall(".//x:word", ns)]
            self.assertTrue(words)
            for index, (word, (x1, y1, x2, y2)) in enumerate(words):
                with self.subTest(page=number, word=word):
                    # Respect the actual 9 mm horizontal / 8 mm vertical margins.
                    self.assertGreaterEqual(x1, 9 * 72 / 25.4 - 0.1)
                    self.assertGreaterEqual(y1, 8 * 72 / 25.4 - 0.1)
                    self.assertLessEqual(x2, width - 9 * 72 / 25.4 + 0.1)
                    self.assertLessEqual(y2, height - 8 * 72 / 25.4 + 0.1)
                    for other, (ox1, oy1, ox2, oy2) in words[index + 1:]:
                        overlap_x = min(x2, ox2) - max(x1, ox1)
                        overlap_y = min(y2, oy2) - max(y1, oy1)
                        self.assertFalse(overlap_x > 0.25 and overlap_y > 0.25,
                                         f"Overlapping text: {word!r} / {other!r}")


if __name__ == "__main__":
    unittest.main(verbosity=2)

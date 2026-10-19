#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Exhaustive published computer8 mapping and its VoiceOver provenance.

No network or Apple installation is required. When VoiceOver's Liblouis
provider is present, compare its actual registered US ASCII definitions too.
"""
from collections import Counter
from pathlib import Path
import plistlib
import unittest

import yaml

ROOT = Path(__file__).resolve().parents[2]
RESEARCH = ROOT / "references/research/computer8.yaml"
VOICEOVER = Path("/System/Library/ScreenReader/BrailleTables/"
                 "LiblouisBrailleTranslator.brailletable/Contents/Resources")


def dot_mask(dots):
    return sum(1 << (dot - 1) for dot in dots)


def ascii_definitions(table):
    """Read only single-cell printable ASCII facts, following relevant includes.

    Not a Liblouis translator: intentionally ignores noback Unicode aliases,
    display rules, extended/control mappings, and the braille-patterns include.
    """
    result = {}
    for line in table.read_text().splitlines():
        fields = line.split()
        if len(fields) == 2 and fields[0] == "include":
            if fields[1] != "braille-patterns.cti":
                result.update(ascii_definitions(table.parent / fields[1]))
            continue
        if len(fields) < 3 or fields[0] not in {
                "lowercase", "uppercase", "digit", "punctuation", "sign", "math", "space"}:
            continue
        char, dots = fields[1:3]
        if char.startswith("\\x"):
            char = chr(int(char[2:], 16))
        elif char == "\\\\":
            char = "\\"
        elif char == "\\s":
            char = " "
        if len(char) == 1 and 0x21 <= ord(char) <= 0x7e and dots.isdigit():
            result[ord(char)] = dot_mask([int(dot) for dot in dots])
    return result


class Computer8Mapping(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.research = yaml.safe_load(RESEARCH.read_text())
        cls.matrix = cls.research["patterns"]
        cls.patterns = [entry for row in cls.matrix for entry in row]
        cls.ascii = {entry: mask for mask, entry in enumerate(cls.patterns)
                     if isinstance(entry, int)}

    def test_all_256_patterns_explicit(self):
        self.assertEqual(len(self.matrix), 16)
        self.assertTrue(all(len(row) == 16 for row in self.matrix))
        self.assertEqual(len(self.patterns), 256)
        self.assertEqual(len(self.ascii), 94)
        self.assertEqual(set(self.ascii), set(range(0x21, 0x7f)))
        self.assertEqual(sum(isinstance(entry, int) for entry in self.patterns), 94)
        self.assertEqual(Counter(entry for entry in self.patterns if isinstance(entry, str)),
                         {"reserved": 160, "delete": 1, "return": 1})
        self.assertEqual(self.patterns[0], "reserved")
        self.assertEqual(self.patterns[0x40], "delete")
        self.assertEqual(self.patterns[0x80], "return")
        self.assertEqual(self.patterns[0xc0], "reserved")

    def test_ascii_fixtures_equal_published_mapping(self):
        cases = yaml.safe_load((ROOT / "tests/braille/computer8/characters.yaml").read_text())["cases"]
        seen = set()
        for case in cases:
            if len(case["inputs"]) != 1 or not case["inputs"][0]:
                continue
            mask = dot_mask(case["inputs"][0])
            char = ord(case["outputs"][0])
            self.assertEqual(self.patterns[mask], char, case["name"])
            self.assertNotIn(char, seen)
            seen.add(char)
        self.assertEqual(seen, set(range(0x21, 0x7f)))

    def test_reserved_fixtures_exhaust_enterable_reserved_patterns(self):
        cases = yaml.safe_load((ROOT / "tests/braille/computer8/reserved.yaml").read_text())["cases"]
        masks = []
        for case in cases:
            self.assertEqual(len(case["inputs"]), 1)
            self.assertTrue(case["inputs"][0])
            self.assertEqual(case["outputs"], [])
            masks.append(dot_mask(case["inputs"][0]))
        self.assertEqual(len(masks), len(set(masks)))
        self.assertEqual(set(masks), {mask for mask, entry in enumerate(self.patterns)
                                     if mask != 0 and entry == "reserved"})
        self.assertEqual(len(masks), 159)

    def test_independent_dot7_and_dot8_policy(self):
        for char in "abcdefghijklmnopqrstuvwxyz":
            self.assertEqual(self.ascii[ord(char.upper())], self.ascii[ord(char)] | 0x40)
        self.assertEqual(self.ascii[ord("`")], 0x08)
        self.assertEqual(self.ascii[ord("|")], 0x33)
        self.assertEqual(self.ascii[ord("_")], 0x38)  # Liblouis, NOT legacy Duxbury
        self.assertTrue(all(mask & 0x80 == 0 for mask in self.ascii.values()))

    @unittest.skipUnless((VOICEOVER / "Tables.plist").exists(),
                         "macOS VoiceOver's Liblouis provider is not installed")
    def test_installed_voiceover_registered_us_ascii(self):
        registration = plistlib.loads((VOICEOVER / "Tables.plist").read_bytes())
        self.assertEqual(registration["eng-xna_8dot"]["table"], "en_US-comp8-ext.tbl")
        self.assertEqual(registration["eng-xna"]["8Dot"], "en_US-comp8-ext.tbl")
        tables = VOICEOVER / "liblouis/tables"
        self.assertEqual(ascii_definitions(tables / "en_US-comp8-ext.tbl"), self.ascii)
        self.assertEqual(ascii_definitions(tables / "en-us-comp8.ctb"), self.ascii)


if __name__ == "__main__":
    unittest.main(verbosity=2)

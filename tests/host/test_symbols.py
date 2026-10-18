#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""The 21 specified UEB1 symbols through production capture/matching/dispatch.

Run the independent C assertions and scoped existing YAML acceptance fixtures
with stub and pinned real-ZMK keycodes. Later contextual cases stay deferred.
"""
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import tempfile
import unittest

import yaml

ROOT = Path(__file__).resolve().parents[2]
ZMK = Path(os.environ.get("ZMK_APP", ROOT / "build/baseline/zmk/app")).resolve()
RESEARCH = ROOT / "references/research/ueb-literary-signs.yaml"
SOURCES = [
    "tests/host/test_symbols.c",
    "src/braille_chord.c", "src/braille_settings.c",
    "src/braille_engine.c", "src/braille_dispatch.c", "src/braille_tables.c",
    "src/braille_rules_grade1.c", "src/braille_rules_nav.c",
    "src/braille_rules_computer8.c", "src/braille_rules_grade2.c",
]
# Independent US host-layout oracle, not the production/stub keycode macros.
HOST_KEYS = {chr(ord("a") + i): 0x070000 | (0x04 + i) for i in range(26)}
HOST_KEYS.update({char: 0x070000 | usage for char, usage in {
    " ": 0x2c, "-": 0x2d, "=": 0x2e, "[": 0x2f, "]": 0x30,
    "\\": 0x31, ",": 0x36, ".": 0x37, "/": 0x38,
}.items()})
HOST_KEYS.update({char: 0x02070000 | usage for char, usage in {
    "@": 0x1f, "#": 0x20, "$": 0x21, "%": 0x22, "^": 0x23,
    "&": 0x24, "*": 0x25, "(": 0x26, ")": 0x27, "_": 0x2d,
    "+": 0x2e, "{": 0x2f, "}": 0x30, ":": 0x33, "~": 0x35,
    "<": 0x36, ">": 0x37,
}.items()})


def cell_mask(cell):
    return sum(1 << (dot - 1) for dot in cell) if cell else 0x100


class SpecifiedMultiCellSymbols(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.signs = yaml.safe_load(RESEARCH.read_text())["normative"]["multi_cell_symbols"]["signs"]
        cls.fixtures = yaml.safe_load((ROOT / "tests/braille/ueb1/punctuation.yaml").read_text())["cases"]

    def check_scoped_fixtures(self, binary):
        selected = []
        for sign in self.signs:
            cases = [case for case in self.fixtures
                     if case["inputs"] == sign["dots"] and case["outputs"] == [sign["print"]]]
            self.assertEqual(len(cases), 1, sign["name"])
            selected.extend(cases)
        # Only examples possible with this leaf's signs and the existing
        # alphabet/core punctuation/space; no numeric/caps/quote implementation.
        names = {
            "example &c (etc)", "example ~s", "example ab/cd",
            "example c:\\desktop", "example idea(s)", "example [be]hold", "case <a>",
        }
        examples = [case for case in self.fixtures if case["name"] in names]
        self.assertEqual({case["name"] for case in examples}, names)
        for case in selected + examples:
            with self.subTest(fixture=case["name"]):
                result = subprocess.check_output([
                    str(binary), "--sequence", *[hex(cell_mask(cell)) for cell in case["inputs"]],
                ], text=True)
                actual = [int(line, 16) for line in result.splitlines()]
                self.assertEqual(actual, [HOST_KEYS[char] for char in case["outputs"]])

    def compile_and_run(self, split, grade2, real_keycodes=False):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            binary = tmp / "symbols"
            includes = []
            if real_keycodes:
                # Keep runtime headers stubbed; dt-bindings alone come from
                # the pinned ZMK checkout, as in an actual firmware build.
                headers = tmp / "headers"
                headers.mkdir()
                (headers / "dt-bindings").symlink_to(ZMK / "include/dt-bindings",
                                                    target_is_directory=True)
                includes = ["-I", str(headers)]
            subprocess.run(shlex.split(os.environ.get("CC", "cc")) + [
                "-std=c11", "-Wall", "-Wextra", "-Werror",
                f"-DCONFIG_ZMK_SPLIT={split}",
                *(["-DCONFIG_ZMK_BRAILLE_GRADE2=1"] if grade2 else []),
                *includes,
                "-I", str(ROOT / "tests/host/stubs"), "-I", str(ROOT / "include"),
                *[str(ROOT / source) for source in SOURCES], "-o", str(binary),
            ], check=True)
            subprocess.run([str(binary)], check=True)
            self.check_scoped_fixtures(binary)

    def test_specified_symbols(self):
        for grade2 in (False, True):
            for split in (0, 1):
                with self.subTest(grade2=grade2, split=split):
                    self.compile_and_run(split, grade2)

    @unittest.skipUnless((ZMK / "include/dt-bindings/zmk/keys.h").exists(),
                         "Pinned ZMK workspace is not available")
    def test_real_zmk_keycodes(self):
        self.compile_and_run(split=1, grade2=False, real_keycodes=True)

    def test_recovered_research_scope_and_patterns(self):
        self.assertEqual(len(self.signs), 21)
        self.assertEqual({sign["print"] for sign in self.signs}, set("&*@^#$%~/_+=<>()[]{}\\"))
        sequences = set()
        for sign in self.signs:
            with self.subTest(sign=sign["name"]):
                self.assertEqual(len(sign["dots"]), 2)
                for cell in sign["dots"]:
                    self.assertEqual(cell, sorted(set(cell)))
                    self.assertTrue(all(1 <= dot <= 6 for dot in cell))
                masks = tuple(cell_mask(cell) for cell in sign["dots"])
                self.assertNotIn(masks, sequences)
                sequences.add(masks)
                self.assertEqual(sign["braille"], "".join(chr(0x2800 + mask) for mask in masks))
                self.assertTrue(sign["ref"]["section"])
                self.assertGreater(sign["ref"]["page"], 0)

    @unittest.skipUnless(shutil.which("pdftotext"), "Poppler's pdftotext is not installed")
    def test_patterns_on_cited_rulebook_pages(self):
        pages = subprocess.check_output([
            "pdftotext", "-layout",
            str(ROOT / "references/iceb/rules-of-unified-english-braille-2024.pdf"), "-",
        ], text=True).split("\f")
        for sign in self.signs:
            with self.subTest(sign=sign["name"]):
                page = sign["ref"]["page"] + 28
                self.assertIn(sign["braille"], pages[page - 1])


if __name__ == "__main__":
    unittest.main(verbosity=2)

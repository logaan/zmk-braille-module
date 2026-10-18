#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Compile-check and run the 26 six-dot alphabet mappings through the real
selection/matching/dispatch path against a recording behavior stub."""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]

SOURCES = [
    "tests/host/test_alphabet.c",
    "src/braille_engine.c",
    "src/braille_dispatch.c",
    "src/braille_tables.c",
    "src/braille_rules_grade1.c",
    "src/braille_rules_nav.c",
    "src/braille_rules_computer8.c",
]


class SixDotAlphabet(unittest.TestCase):
    def test_map_the_26_six_dot_alphabet_cells(self):
        for split in (0, 1):
            with self.subTest(split=split), tempfile.TemporaryDirectory() as tmp:
                binary = Path(tmp) / "alphabet"
                subprocess.run(shlex.split(os.environ.get("CC", "cc")) + [
                    "-std=c11", "-Wall", "-Wextra", "-Werror", f"-DCONFIG_ZMK_SPLIT={split}",
                    "-I", str(ROOT / "tests/host/stubs"), "-I", str(ROOT / "include"),
                    *[str(ROOT / s) for s in SOURCES], "-o", str(binary),
                ], check=True)
                subprocess.run([str(binary)], check=True)


if __name__ == "__main__":
    unittest.main(verbosity=2)

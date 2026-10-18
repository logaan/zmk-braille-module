#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Compile-check and run the named character-table registry against the real
compiled tables, without and with the optional Grade 2 rules, and with
devicetree-default overrides."""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]

SOURCES = [
    "tests/host/test_tables.c",
    "src/braille_tables.c",
    "src/braille_engine.c",
    "src/braille_rules_grade1.c",
    "src/braille_rules_nav.c",
    "src/braille_rules_computer8.c",
    "src/braille_rules_grade2.c",
]

VARIANTS = {
    "base": [],
    "grade2": ["-DCONFIG_ZMK_BRAILLE_GRADE2=1"],
    "default-computer8": [
        '-DBRAILLE_TABLES_DEFAULT_MODE="computer8"',
        '-DBRAILLE_TABLES_TEST_DEFAULT="computer8"',
    ],
    # The devicetree default names a table that is not compiled in.
    "default-uncompiled": [
        '-DBRAILLE_TABLES_DEFAULT_MODE="ueb2"',
        "-DBRAILLE_TABLES_TEST_DEFAULT_UNCOMPILED=1",
    ],
}


class TableRegistry(unittest.TestCase):
    def test_register_extensible_named_character_tables(self):
        for name, extra in VARIANTS.items():
            for split in (0, 1):
                with self.subTest(variant=name, split=split), tempfile.TemporaryDirectory() as tmp:
                    binary = Path(tmp) / "tables"
                    subprocess.run(shlex.split(os.environ.get("CC", "cc")) + [
                        "-std=c11", "-Wall", "-Wextra", "-Werror",
                        f"-DCONFIG_ZMK_SPLIT={split}",
                        "-I", str(ROOT / "tests/host/stubs"), "-I", str(ROOT / "include"),
                        *[str(ROOT / s) for s in SOURCES], *extra, "-o", str(binary),
                    ], check=True)
                    subprocess.run([str(binary)], check=True)


if __name__ == "__main__":
    unittest.main(verbosity=2)

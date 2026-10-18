#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Real command tables through capture, selection, matching and dispatch.

Only behavior invocation is recorded here. Real ZMK sticky/HID semantics need
native firmware integration tests, not a Python or C sticky-key simulation.
"""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
SOURCES = [
    "tests/host/test_commands.c",
    "src/braille_chord.c", "src/braille_settings.c",
    "src/braille_engine.c", "src/braille_dispatch.c", "src/braille_tables.c",
    "src/braille_rules_grade1.c", "src/braille_rules_nav.c",
    "src/braille_rules_computer8.c", "src/braille_rules_grade2.c",
]


class FactoryCommands(unittest.TestCase):
    def test_factory_commands(self):
        for grade2 in (False, True):
            for split in (0, 1):
                with self.subTest(grade2=grade2, split=split), tempfile.TemporaryDirectory() as tmp:
                    binary = Path(tmp) / "commands"
                    subprocess.run(shlex.split(os.environ.get("CC", "cc")) + [
                        "-std=c11", "-Wall", "-Wextra", "-Werror",
                        f"-DCONFIG_ZMK_SPLIT={split}",
                        *(["-DCONFIG_ZMK_BRAILLE_GRADE2=1"] if grade2 else []),
                        "-I", str(ROOT / "tests/host/stubs"), "-I", str(ROOT / "include"),
                        *[str(ROOT / source) for source in SOURCES], "-o", str(binary),
                    ], check=True)
                    subprocess.run([str(binary)], check=True)

    def test_basic_consumer_report_warns(self):
        with tempfile.TemporaryDirectory() as tmp:
            result = subprocess.run(shlex.split(os.environ.get("CC", "cc")) + [
                "-std=c11", "-DCONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_BASIC=1",
                "-I", str(ROOT / "tests/host/stubs"), "-I", str(ROOT / "include"),
                "-c", str(ROOT / "src/braille_rules_nav.c"),
                "-o", str(Path(tmp) / "commands.o"),
            ], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("BASIC consumer usages cannot send Globe; use FULL",
                          result.stderr)


if __name__ == "__main__":
    unittest.main(verbosity=2)

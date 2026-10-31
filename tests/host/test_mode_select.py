# SPDX-License-Identifier: MIT
"""Actual runtime selector with production capture, engine and mode tables."""
import itertools
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]

class ModeSelect(unittest.TestCase):
    def test_runtime_selection(self):
        sources = ["tests/host/test_mode_select.c"] + [f"src/{name}.c" for name in (
            "braille_chord", "braille_settings", "braille_engine", "braille_dispatch",
            "braille_tables", "braille_rules_grade1", "braille_rules_computer8",
            "braille_rules_nav", "braille_rules_grade2")]
        for grade2, metadata, split in itertools.product((0, 1), repeat=3):
            with self.subTest(grade2=grade2, metadata=metadata, split=split), tempfile.TemporaryDirectory() as tmp:
                binary = Path(tmp) / "select"
                subprocess.run(shlex.split(os.environ.get("CC", "cc")) + [
                    "-std=c11", "-Wall", "-Wextra", "-Werror",
                    f"-DCONFIG_ZMK_SPLIT={split}",
                    f"-DCONFIG_ZMK_BEHAVIOR_METADATA={metadata}",
                    *(["-DCONFIG_ZMK_BRAILLE_GRADE2=1"] if grade2 else []),
                    "-I", str(ROOT / "include"), "-I", str(ROOT / "tests/host/stubs"),
                    *[str(ROOT / s) for s in sources], "-o", str(binary),
                ], check=True)
                subprocess.run([str(binary)], check=True)

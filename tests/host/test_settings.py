#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Settings lifecycle with real capture/gate and a fixture pending consumer."""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]


class Settings(unittest.TestCase):
    def test_lifecycle(self):
        for split in (0, 1):
            with self.subTest(split=split), tempfile.TemporaryDirectory() as tmp:
                binary = Path(tmp) / "settings"
                subprocess.run(shlex.split(os.environ.get("CC", "cc")) + [
                    "-std=c11", "-Wall", "-Wextra", "-Werror", f"-DCONFIG_ZMK_SPLIT={split}",
                    "-I", str(ROOT / "tests/host/stubs"), "-I", str(ROOT / "include"),
                    str(ROOT / "tests/host/test_settings.c"), str(ROOT / "src/braille_chord.c"),
                    str(ROOT / "src/braille_settings.c"), "-o", str(binary),
                ], check=True)
                subprocess.run([str(binary)], check=True)


if __name__ == "__main__":
    unittest.main(verbosity=2)

#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Contextual UEB1 through the real capture/matching/dispatch path."""
import json
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

from test_computer8 import HOST_KEYS, cell_mask

ROOT = Path(__file__).resolve().parents[2]
ZMK = Path(os.environ.get("ZMK_APP", ROOT / "build/baseline/zmk/app")).resolve()
SOURCES = [
    "tests/host/test_ueb1.c", "src/braille_chord.c", "src/braille_settings.c",
    "src/braille_engine.c", "src/braille_dispatch.c", "src/braille_tables.c",
    "src/braille_rules_grade1.c", "src/braille_rules_nav.c",
    "src/braille_rules_computer8.c", "src/braille_rules_grade2.c",
]


def observe(binary, masks):
    return json.loads(subprocess.check_output(
        [str(binary), "--sequence", *map(hex, masks)], text=True))


class ContextualUEB1(unittest.TestCase):
    def compile_and_run(self, split, grade2, real_keycodes=False):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            includes = []
            if real_keycodes:
                headers = tmp / "headers"
                headers.mkdir()
                (headers / "dt-bindings").symlink_to(ZMK / "include/dt-bindings", target_is_directory=True)
                includes = ["-I", str(headers)]
            binary = tmp / "ueb1"
            subprocess.run(shlex.split(os.environ.get("CC", "cc")) + [
                "-std=c11", "-Wall", "-Wextra", "-Werror", f"-DCONFIG_ZMK_SPLIT={split}",
                *(["-DCONFIG_ZMK_BRAILLE_GRADE2=1"] if grade2 else []), *includes,
                "-I", str(ROOT / "tests/host/stubs"), "-I", str(ROOT / "include"),
                *[str(ROOT / source) for source in SOURCES], "-o", str(binary),
            ], check=True)
            subprocess.run([str(binary)], check=True)

    def test_contextual_rules(self):
        for split in (0, 1):
            for grade2 in (False, True):
                with self.subTest(split=split, grade2=grade2):
                    self.compile_and_run(split, grade2)

    @unittest.skipUnless((ZMK / "include/dt-bindings/zmk/keys.h").exists(), "Pinned ZMK unavailable")
    def test_real_zmk_keycodes(self):
        self.compile_and_run(split=1, grade2=False, real_keycodes=True)


if __name__ == "__main__":
    unittest.main(verbosity=2)

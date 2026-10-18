#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Seven context-free UEB1 signs through production capture/matching/dispatch.

The real pinned ZMK keycode header is also checked when the baseline workspace
is available, so implicit Shift values are not justified only by host stubs.
"""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
ZMK = Path(os.environ.get("ZMK_APP", ROOT / "build/baseline/zmk/app")).resolve()
SOURCES = [
    "tests/host/test_punctuation.c",
    "src/braille_chord.c", "src/braille_settings.c",
    "src/braille_engine.c", "src/braille_dispatch.c", "src/braille_tables.c",
    "src/braille_rules_grade1.c", "src/braille_rules_nav.c",
    "src/braille_rules_computer8.c", "src/braille_rules_grade2.c",
]


class CorePunctuation(unittest.TestCase):
    def compile_and_run(self, split, grade2, real_keycodes=False):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            binary = tmp / "punctuation"
            includes = []
            if real_keycodes:
                # Keep behavior/runtime headers stubbed; only dt-bindings come
                # from the real ZMK checkout, exactly as a firmware build does.
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

    def test_context_free_core_punctuation(self):
        for grade2 in (False, True):
            for split in (0, 1):
                with self.subTest(grade2=grade2, split=split):
                    self.compile_and_run(split, grade2)

    @unittest.skipUnless((ZMK / "include/dt-bindings/zmk/keys.h").exists(),
                         "Pinned ZMK workspace is not available")
    def test_real_zmk_keycodes(self):
        self.compile_and_run(split=1, grade2=False, real_keycodes=True)


if __name__ == "__main__":
    unittest.main(verbosity=2)

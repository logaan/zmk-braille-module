#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Exhaust computer8 through production capture/matching/dispatch.

Checks raw HID usages, all published patterns and all existing character,
reserved, and isolation YAML cases. Firmware uses the actual pinned ZMK
keycode header; exercise that header here as well when it is available.
"""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

import yaml

ROOT = Path(__file__).resolve().parents[2]
ZMK = Path(os.environ.get("ZMK_APP", ROOT / "build/baseline/zmk/app")).resolve()
SOURCES = [
    "tests/host/test_computer8.c",
    "src/braille_chord.c", "src/braille_settings.c",
    "src/braille_engine.c", "src/braille_dispatch.c", "src/braille_tables.c",
    "src/braille_rules_grade1.c", "src/braille_rules_nav.c",
    "src/braille_rules_computer8.c", "src/braille_rules_grade2.c",
]
# Independent US host-layout oracle, not production/stub keycode macros.
HOST_KEYS = {chr(ord("a") + i): 0x070004 + i for i in range(26)}
HOST_KEYS.update({chr(ord("A") + i): 0x02070004 + i for i in range(26)})
HOST_KEYS.update({char: 0x07001e + i for i, char in enumerate("1234567890")})
HOST_KEYS.update({char: 0x070000 | usage for char, usage in {
    " ": 0x2c, "-": 0x2d, "=": 0x2e, "[": 0x2f, "]": 0x30, "\\": 0x31,
    ";": 0x33, "'": 0x34, "`": 0x35, ",": 0x36, ".": 0x37, "/": 0x38,
}.items()})
HOST_KEYS.update({char: 0x02070000 | usage for char, usage in {
    "!": 0x1e, "@": 0x1f, "#": 0x20, "$": 0x21, "%": 0x22, "^": 0x23,
    "&": 0x24, "*": 0x25, "(": 0x26, ")": 0x27, "_": 0x2d, "+": 0x2e,
    "{": 0x2f, "}": 0x30, "|": 0x31, ":": 0x33, '"': 0x34, "~": 0x35,
    "<": 0x36, ">": 0x37, "?": 0x38,
}.items()})


def cell_mask(cell):
    return sum(1 << (dot - 1) for dot in cell) if cell else 0x100


class Computer8Characters(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        research = yaml.safe_load((ROOT / "references/research/computer8.yaml").read_text())
        cls.patterns = [entry for row in research["patterns"] for entry in row]
        cls.fixtures = []
        for name in ("characters", "reserved", "isolation"):
            cls.fixtures.extend(yaml.safe_load(
                (ROOT / f"tests/braille/computer8/{name}.yaml").read_text())["cases"])

    def check_sequence(self, binary, masks, keys, notices=0):
        result = subprocess.check_output([str(binary), "--sequence", *map(hex, masks)], text=True)
        lines = result.splitlines()
        self.assertEqual(lines[-1], f"notices {notices}")
        self.assertEqual([int(line.removeprefix("key "), 16) for line in lines[:-1]], keys)

    def check_published_map_and_fixtures(self, binary):
        self.assertEqual(set(HOST_KEYS), {chr(code) for code in range(0x20, 0x7f)})
        for mask, entry in enumerate(self.patterns):
            with self.subTest(mask=hex(mask)):
                if isinstance(entry, int):
                    keys = [HOST_KEYS[chr(entry)]]
                else:
                    keys = {"reserved": [], "delete": [0x07002a], "return": [0x070028]}[entry]
                self.check_sequence(binary, [mask], keys, int(mask != 0 and entry == "reserved"))
        for case in self.fixtures:
            with self.subTest(fixture=case["name"]):
                masks = [cell_mask(cell) for cell in case["inputs"]]
                notices = sum(mask < 256 and self.patterns[mask] == "reserved" for mask in masks)
                self.check_sequence(binary, masks, [HOST_KEYS[c] for c in case["outputs"]], notices)

    def compile_and_run(self, split, grade2, real_keycodes=False):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            binary = tmp / "computer8"
            includes = []
            if real_keycodes:
                headers = tmp / "headers"
                headers.mkdir()
                (headers / "dt-bindings").symlink_to(ZMK / "include/dt-bindings", target_is_directory=True)
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
            self.check_published_map_and_fixtures(binary)

    def test_all_patterns_and_ascii(self):
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

#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Thin driver/edge-boundary tests; scripts/test-routing exercises real ZMK."""
import itertools
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
CC = shlex.split(os.environ.get("CC", "cc"))
FLAGS = ["-std=c11", "-Wall", "-Wextra", "-Werror",
         "-I", str(ROOT / "tests/host/stubs"), "-I", str(ROOT / "include")]


class Routing(unittest.TestCase):
    def test_edges_and_parameters(self):
        for split, metadata in itertools.product((0, 1), repeat=2):
            with self.subTest(split=split, metadata=metadata), tempfile.TemporaryDirectory() as tmp:
                binary = Path(tmp) / "routing"
                subprocess.run(CC + FLAGS + [
                    f"-DCONFIG_ZMK_SPLIT={split}", f"-DCONFIG_ZMK_BEHAVIOR_METADATA={metadata}",
                    str(ROOT / "tests/host/test_behavior_routing.c"),
                    str(ROOT / "src/braille_settings.c"), "-o", str(binary),
                ], check=True)
                subprocess.run([str(binary)], check=True)

    def test_independent_compatibles_and_unavailable_consumer(self):
        # Every node combination, including none and a dot behavior without space.
        for nodes in itertools.product((0, 1), repeat=4):
            for metadata in (0, 1):
                with self.subTest(nodes=nodes, metadata=metadata), tempfile.TemporaryDirectory() as tmp:
                    binary = Path(tmp) / "registration"
                    defines = [f"-DTEST_{name}_NODES={value}" for name, value in
                               zip(("DOT", "SPACE", "MODE", "SETTINGS"), nodes)]
                    subprocess.run(CC + FLAGS + defines + [
                        f"-DCONFIG_ZMK_BEHAVIOR_METADATA={metadata}",
                        str(ROOT / "tests/host/test_behavior_scaffold.c"),
                        str(ROOT / "src/braille_chord.c"), str(ROOT / "src/braille_settings.c"),
                        "-o", str(binary),
                    ], check=True)
                    subprocess.run([str(binary)], check=True)


if __name__ == "__main__":
    unittest.main(verbosity=2)

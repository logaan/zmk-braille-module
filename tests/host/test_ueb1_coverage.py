#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Audit all 95 printable US keys through production UEB1 rules."""
from pathlib import Path
import tempfile
import unittest

import yaml
from test_computer8 import HOST_KEYS, cell_mask
from test_ueb1 import build, observe, ZMK

ROOT = Path(__file__).resolve().parents[2]


class Grade1KeyboardCoverage(unittest.TestCase):
    def test_all_printable_us_characters(self):
        research = ROOT / "references/research"
        literary = yaml.safe_load((research / "ueb-literary-signs.yaml").read_text())["normative"]
        capitals = yaml.safe_load((research / "ueb-capitalisation.yaml").read_text())["normative"]
        numeric = yaml.safe_load((research / "ueb-numeric-mode.yaml").read_text())["normative"]
        signs = literary["alphabet"]["signs"] + literary["keyboard_coverage"]["characters"]
        signs += capitals["capital_letters"]["signs"] + numeric["numeric_indicators"]["signs"][:10]
        signs += [{"print": " ", "dots": [[]]}]
        self.assertEqual(len(signs), 95)
        self.assertEqual({sign["print"] for sign in signs}, set(HOST_KEYS))
        with tempfile.TemporaryDirectory() as tmp:
            binary = build(tmp, real_keycodes=(ZMK / "include/dt-bindings/zmk/keys.h").exists())
            for sign in signs:
                with self.subTest(character=sign["print"]):
                    cells = sign["dots"]
                    self.assertTrue(all(1 <= dot <= 6 for cell in cells for dot in cell))
                    if sign["print"] == "?":
                        cells = [[5, 6], *cells]  # question at reset needs Grade 1 (7.5.3/7.6.9)
                    masks = [cell_mask(cell) for cell in cells]
                    result = observe(binary, masks)
                    self.assertEqual(result["keys"], [HOST_KEYS[sign["print"]]])
                    self.assertEqual(result["pending"], [])
                    self.assertEqual(result["drops"], [])
                    # Space resolves context before the following lowercase key.
                    result = observe(binary, [*masks, 0x100, 0x01])
                    self.assertEqual(result["keys"], [HOST_KEYS[sign["print"]], HOST_KEYS[" "], HOST_KEYS["a"]])
                    self.assertEqual(result["pending"], [])
                    self.assertEqual(result["drops"], [])


if __name__ == "__main__":
    unittest.main(verbosity=2)

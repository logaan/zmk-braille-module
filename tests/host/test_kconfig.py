#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Use kconfiglib from the pinned Zephyr checkout (see README)."""
from pathlib import Path
import tempfile
import unittest

import kconfiglib

ROOT = Path(__file__).resolve().parents[2]


class KconfigContract(unittest.TestCase):
    def config(self, split, central, enabled, grade2):
        with tempfile.TemporaryDirectory() as tmp:
            wrapper = Path(tmp) / "Kconfig"
            wrapper.write_text(f'''
mainmenu "Braille configuration test"
config ZMK_SPLIT
    bool "Split"
config ZMK_SPLIT_ROLE_CENTRAL
    bool "Central"
source "{ROOT / 'Kconfig'}"
''')
            config = kconfiglib.Kconfig(str(wrapper), warn_to_stderr=False)
            for symbol, value in (("ZMK_SPLIT", split), ("ZMK_SPLIT_ROLE_CENTRAL", central),
                                  ("ZMK_FEATURE_BRAILLE", enabled), ("ZMK_BRAILLE_GRADE2", grade2)):
                if value is not None:
                    config.syms[symbol].set_value("y" if value else "n")
            return config

    def test_central_only_and_optional_grade2(self):
        for split in (False, True):
            for central in (False, True):
                for enabled in (False, True):
                    for grade2 in (False, True):
                        with self.subTest(split=split, central=central, enabled=enabled, grade2=grade2):
                            config = self.config(split, central, enabled, grade2)
                            available = enabled and (not split or central)
                            self.assertEqual(config.syms["ZMK_FEATURE_BRAILLE"].str_value,
                                             "y" if available else "n")
                            self.assertEqual(config.syms["ZMK_BRAILLE_GRADE2"].str_value,
                                             "y" if available and grade2 else "n")
                            self.assertEqual(config.warnings, [])

    def test_disabled_defaults(self):
        config = self.config(False, False, None, None)
        self.assertEqual(config.syms["ZMK_FEATURE_BRAILLE"].str_value, "n")
        self.assertEqual(config.syms["ZMK_BRAILLE_GRADE2"].str_value, "n")


if __name__ == "__main__":
    unittest.main(verbosity=2)

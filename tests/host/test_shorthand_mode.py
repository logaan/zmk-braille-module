# SPDX-License-Identifier: MIT
"""Saved fourth-mode restoration and real contextual display snapshots."""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest
ROOT = Path(__file__).resolve().parents[2]
class ShorthandMode(unittest.TestCase):
    def test_saved_mode_and_preview_with_feature_enabled_or_missing(self):
        sources = ['tests/host/test_shorthand_mode.c'] + ['src/braille_' + name + '.c' for name in
            ('storage', 'display', 'chord', 'settings', 'engine', 'tables', 'rules_grade1',
             'rules_computer8', 'rules_nav', 'rules_grade2')]
        for enabled in (False, True):
            with self.subTest(enabled=enabled), tempfile.TemporaryDirectory() as tmp:
                binary = Path(tmp)/'mode'
                subprocess.run(shlex.split(os.environ.get('CC', 'cc')) +
                    ['-std=c11', '-Wall', '-Wextra', '-Werror', '-DCONFIG_ZMK_BRAILLE_STORAGE=1',
                     '-DCONFIG_ZMK_BRAILLE_GRADE2=1',
                     *(['-DCONFIG_ZMK_BRAILLE_COMPUTER8_SHORT=1'] if enabled else []),
                     '-I', str(ROOT/'include'), '-I', str(ROOT/'tests/host/stubs')]+
                    [str(ROOT/s) for s in sources]+['-o', str(binary)], check=True)
                subprocess.run([str(binary)], check=True)
if __name__ == '__main__': unittest.main()

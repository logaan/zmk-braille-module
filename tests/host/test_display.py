# SPDX-License-Identifier: MIT
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest
ROOT = Path(__file__).resolve().parents[2]
class Display(unittest.TestCase):
    def test_read_only_contextual_preview(self):
        with tempfile.TemporaryDirectory() as tmp:
            exe = Path(tmp) / 'display'
            sources = ['tests/host/test_display.c'] + ['src/braille_' + name + '.c' for name in
                ('display','chord','settings','engine','tables','rules_grade1','rules_computer8','rules_nav')]
            subprocess.run(shlex.split(os.environ.get('CC','cc')) +
                ['-std=c11','-Wall','-Wextra','-Werror','-I',str(ROOT/'include'),'-I',str(ROOT/'tests/host/stubs')] +
                [str(ROOT/s) for s in sources] + ['-o',str(exe)], check=True)
            subprocess.run([str(exe)], check=True)

    def test_unchanged_frames_and_all_cache_invalidations(self):
        with tempfile.TemporaryDirectory() as tmp:
            exe = Path(tmp) / 'display-cache'
            subprocess.run(shlex.split(os.environ.get('CC','cc')) +
                ['-std=c11','-Wall','-Wextra','-Werror','-DCONFIG_ZMK_BRAILLE_STORAGE=1',
                 '-I',str(ROOT/'include'),'-I',str(ROOT/'tests/host/stubs'),
                 str(ROOT/'src/braille_display.c'),str(ROOT/'tests/host/test_display_cache.c'),
                 '-o',str(exe)],check=True)
            subprocess.run([str(exe)],check=True)

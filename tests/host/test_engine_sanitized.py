# SPDX-License-Identifier: MIT
"""Address/undefined-behavior checks on deterministic bounded random input."""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[2]
class SanitizedEngine(unittest.TestCase):
    def test_randomized_modes_boundaries_and_preview(self):
        with tempfile.TemporaryDirectory() as tmp:
            exe=Path(tmp)/'sanitized'
            sources=['tests/host/test_engine_randomized.c']+['src/braille_'+s+'.c' for s in
                ('engine','tables','rules_grade1','rules_grade2','rules_computer8','rules_nav')]
            subprocess.run(shlex.split(os.environ.get('CC','cc'))+[
                '-std=c11','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer',
                '-DCONFIG_ZMK_BRAILLE_GRADE2=1','-I',str(ROOT/'include'),
                '-I',str(ROOT/'tests/host/stubs'),*[str(ROOT/s) for s in sources],'-o',str(exe)],check=True)
            subprocess.run([str(exe)],check=True,timeout=60)

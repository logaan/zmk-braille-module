# SPDX-License-Identifier: MIT
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[2]
class Editor(unittest.TestCase):
    def test_isolated_settings_workflow(self):
        with tempfile.TemporaryDirectory() as tmp:
            exe=Path(tmp)/'editor'
            sources=['tests/host/test_editor.c']+['src/braille_'+s+'.c' for s in
                ('editor','search','settings','chord','engine','tables','display','rules_grade1','rules_nav','rules_computer8')]
            subprocess.run(shlex.split(os.environ.get('CC','cc'))+['-std=c11','-Wall','-Wextra','-Werror',
                '-I',str(ROOT/'include'),'-I',str(ROOT/'tests/host/stubs')]+[str(ROOT/s) for s in sources]+['-o',str(exe)],check=True)
            subprocess.run([str(exe)],check=True)
    def test_compiled_registry_metadata_filtering(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp=Path(tmp);exe=tmp/'registry'
            (tmp/'braille_keycodes.inc').write_text('{"A",0x70004}, {"TAB",0x7002b},')
            subprocess.run(shlex.split(os.environ.get('CC','cc'))+['-std=c11','-Wall','-Wextra','-Werror',
                '-DCONFIG_ZMK_BRAILLE_SETTINGS_UI=1','-I',str(tmp),'-I',str(ROOT/'tests/host/editor-stubs'),
                '-I',str(ROOT/'include'),'-I',str(ROOT/'tests/host/stubs'),
                str(ROOT/'src/braille_editor_zmk.c'),str(ROOT/'tests/host/test_editor_registry.c'),'-o',str(exe)],check=True)
            subprocess.run([str(exe)],check=True)
    def test_generate_from_selected_zmk_header(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp=Path(tmp);header=tmp/'keys.h';output=tmp/'keys.inc'
            header.write_text('#define A (0x70004)\n#define LEFT_ARROW (0x70050)\n#define LC(k) (k)\n')
            subprocess.run(['python3',str(ROOT/'scripts/generate-braille-keycodes.py'),str(header),str(output)],check=True)
            self.assertEqual(output.read_text().count('},'),2)
            self.assertNotIn('"LC"',output.read_text())
if __name__=='__main__': unittest.main()

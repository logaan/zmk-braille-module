import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[2]
class SmallScreen(unittest.TestCase):
 def test_portrait_and_landscape_pagination(self):
  with tempfile.TemporaryDirectory() as tmp:
   out=Path(tmp)/'small-screen'
   subprocess.run(shlex.split(os.environ.get('CC','cc'))+['-std=c11','-Wall','-Wextra','-Werror','-I',str(ROOT/'include'),'-I',str(ROOT/'tests/host/stubs'),str(ROOT/'tests/host/test_small_screen.c'),str(ROOT/'src/braille_small_screen.c'),'-o',str(out)],check=True)
   subprocess.run([str(out)],check=True)
if __name__=='__main__':unittest.main()

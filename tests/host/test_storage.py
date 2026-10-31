"""Exercise persistent storage with power cycles and faulting flash/device backends."""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest
ROOT = Path(__file__).resolve().parents[2]
class Storage(unittest.TestCase):
    def test_storage_lifecycle(self):
        with tempfile.TemporaryDirectory() as tmp:
            binary=Path(tmp)/"storage"
            subprocess.run(shlex.split(os.environ.get("CC","cc"))+[
                "-std=c11","-Wall","-Wextra","-Werror","-DCONFIG_ZMK_BRAILLE_STORAGE=1",
                "-I",str(ROOT/"include"),"-I",str(ROOT/"tests/host/stubs"),
                str(ROOT/"src/braille_storage.c"),str(ROOT/"tests/host/test_storage.c"),"-o",str(binary)],check=True)
            subprocess.run([str(binary)],check=True)
    def test_shared_matcher_and_compact_pool(self):
        with tempfile.TemporaryDirectory() as tmp:
            binary=Path(tmp)/"storage-engine"
            sources=["tests/host/test_storage_engine.c","src/braille_storage.c","src/braille_engine.c","src/braille_tables.c","src/braille_rules_grade1.c","src/braille_rules_computer8.c","src/braille_rules_nav.c"]
            subprocess.run(shlex.split(os.environ.get("CC","cc"))+[
                "-std=c11","-Wall","-Wextra","-Werror","-DCONFIG_ZMK_BRAILLE_STORAGE=1",
                "-I",str(ROOT/"include"),"-I",str(ROOT/"tests/host/stubs"),
                *[str(ROOT/p) for p in sources],"-o",str(binary)],check=True)
            subprocess.run([str(binary)],check=True)
if __name__=="__main__": unittest.main()

#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Host-only build contract checks. Real Zephyr builds remain authoritative."""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
SOURCES = {
    "src/behavior_braille.c", "src/braille_bindings.c", "src/braille_chord.c", "src/braille_engine.c",
    "src/braille_dispatch.c",
    "src/braille_settings.c",
    "src/braille_rules_grade1.c", "src/braille_rules_nav.c", "src/braille_rules_computer8.c",
}


class BuildContract(unittest.TestCase):
    def test_cmake_source_selection(self):
        for enabled, grade2 in ((False, False), (False, True), (True, False), (True, True)):
            with self.subTest(enabled=enabled, grade2=grade2), tempfile.TemporaryDirectory() as tmp:
                output = Path(tmp) / "selection"
                script = Path(tmp) / "check.cmake"
                script.write_text(f'''
set(APPLICATION_SOURCE_DIR /test-zmk/app)
set(CONFIG_ZMK_FEATURE_BRAILLE {"ON" if enabled else "OFF"})
set(CONFIG_ZMK_BRAILLE_GRADE2 {"ON" if grade2 else "OFF"})
function(zephyr_library)
  file(APPEND "{output}" "library\\n")
endfunction()
function(zephyr_library_include_directories)
  file(APPEND "{output}" "include:${{ARGV}}\\n")
endfunction()
function(zephyr_library_sources)
  foreach(src IN LISTS ARGV)
    file(APPEND "{output}" "${{src}}\\n")
  endforeach()
endfunction()
function(zephyr_library_sources_ifdef symbol)
  if(${{symbol}})
    zephyr_library_sources(${{ARGN}})
  endif()
endfunction()
include("{ROOT / 'CMakeLists.txt'}")
''')
                subprocess.run(["cmake", "-P", str(script)], check=True)
                selected = set(output.read_text().splitlines()) if output.exists() else set()
                expected = SOURCES | {"library", "include:include;/test-zmk/app/include"} if enabled else set()
                if enabled and grade2:
                    expected.add("src/braille_rules_grade2.c")
                self.assertEqual(selected, expected)

    def test_host_c_build_and_registration(self):
        cc = shlex.split(os.environ.get("CC", "cc"))
        with tempfile.TemporaryDirectory() as tmp:
            for no_nodes in (0, 1):
                binary = Path(tmp) / f"behavior-{no_nodes}"
                subprocess.run(cc + [
                    "-std=c11", "-Wall", "-Wextra", "-Werror",
                    f"-DTEST_NO_NODES={no_nodes}", "-I", str(ROOT / "tests/host/stubs"),
                    "-I", str(ROOT / "include"),
                    str(ROOT / "tests/host/test_behavior_scaffold.c"),
                    str(ROOT / "src/braille_chord.c"), str(ROOT / "src/braille_settings.c"),
                    "-o", str(binary),
                ], check=True)
                subprocess.run([str(binary)], check=True)
            for source in sorted((ROOT / "src").glob("braille_*.c")):
                if source.name == "braille_bindings.c":
                    # Compiled against real generated DT headers by test_devicetree.py.
                    continue
                subprocess.run(cc + [
                    "-std=c11", "-Wall", "-Wextra", "-Werror", "-I", str(ROOT / "include"),
                    "-I", str(ROOT / "tests/host/stubs"),
                    "-c", str(source), "-o", str(Path(tmp) / (source.stem + ".o")),
                ], check=True)


if __name__ == "__main__":
    unittest.main(verbosity=2)

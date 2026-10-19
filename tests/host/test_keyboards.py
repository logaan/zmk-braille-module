#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Parse the actual sibling keymaps with release Zephyr, without hardware stubs.

Requires the same workspace/PyYAML as test_devicetree.py and sibling checkouts.
Override QBF (basic-zmk directory), TOUCAN, ZEPHYR_BASE or ZMK_APP if necessary.
"""
import os
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile
import unittest
import yaml

ROOT = Path(__file__).resolve().parents[2]
ZEPHYR = Path(os.environ.get("ZEPHYR_BASE", ROOT / "build/baseline/zephyr")).resolve()
ZMK = Path(os.environ.get("ZMK_APP", ROOT / "build/baseline/zmk/app")).resolve()
QBF = Path(os.environ.get("QBF", ROOT.parent / "qbf-paintbrushes/basic-zmk")).resolve()
TOUCAN = Path(os.environ.get("TOUCAN", ROOT.parent / "zmk-keyboard-toucan")).resolve()
sys.path.insert(0, str(ROOT / "tests/firmware"))
from check_smoke import check_keymap


class KeyboardContract(unittest.TestCase):
    def parse_keymap(self, keyboard, mode=None):
        keymap = QBF / "config/qbf_paintbrush.keymap" if keyboard == "qbf" else TOUCAN / "config/toucan.keymap"
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            source = tmp / "test.dts"
            source.write_text(f'/dts-v1/;\n#include "{keymap}"\n' +
                              (f'&braille {{ default-mode = "{mode}"; }};\n' if mode else ""))
            preprocessed = tmp / "test.pre.dts"
            with preprocessed.open("w") as output:
                subprocess.run(shlex.split(os.environ.get("CC", "cc")) + [
                    "-E", "-P", "-x", "assembler-with-cpp", "-nostdinc", "-undef",
                    "-DCONFIG_ZMK_STUDIO=1", "-DCONFIG_ZMK_POINTING=1",
                ] + [arg for inc in (ROOT / "dts", ZMK / "dts", ZMK / "include", ZEPHYR / "dts", ZEPHYR / "include")
                     for arg in ("-I", str(inc))] + [str(source)], stdout=output, check=True)
            result = subprocess.run([
                sys.executable, str(ZEPHYR / "scripts/dts/gen_defines.py"),
                "--dts", str(preprocessed), "--dtc-flags", "",
                "--bindings-dirs", str(ROOT / "dts/bindings"), str(ZMK / "dts/bindings"), str(ZEPHYR / "dts/bindings"),
                "--header-out", str(tmp / "generated.h"), "--dts-out", str(tmp / "resolved.dts"),
                "--edt-pickle-out", str(tmp / "edt.pickle"),
            ], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            return (tmp / "generated.h").read_text()

    def test_actual_perkins_layouts_and_default_mode(self):
        for keyboard in ("qbf", "toucan"):
            with self.subTest(keyboard=keyboard):
                check_keymap(self.parse_keymap(keyboard), keyboard)

    def test_configured_mode_preserves_both_layouts(self):
        for keyboard in ("qbf", "toucan"):
            with self.subTest(keyboard=keyboard):
                check_keymap(self.parse_keymap(keyboard, "computer8"), keyboard, "computer8")

    def test_matching_published_module_and_release_pins(self):
        projects = []
        for path in (QBF / "config/west.yml", TOUCAN / "config/west.yml", ROOT / "west/west.yml"):
            data = yaml.safe_load(path.read_text())["manifest"]["projects"]
            projects.append({project["name"]: project for project in data})
            self.assertEqual(projects[-1]["zmk"]["revision"], "v0.3.0")
        qbf, toucan, _ = projects
        self.assertEqual(qbf["zmk-braille-module"], toucan["zmk-braille-module"])
        module = qbf["zmk-braille-module"]
        self.assertEqual(module["url"], "https://github.com/logaan/zmk-braille-module")
        self.assertRegex(module["revision"], r"^[0-9a-f]{40}$")
        # Pin must contain the production include, not just a registration stub.
        result = subprocess.run(["git", "-C", str(ROOT), "cat-file", "-e",
                                 module["revision"] + ":dts/behaviors/braille.dtsi"])
        self.assertEqual(result.returncode, 0)

    def test_qbf_legends_track_base_layout(self):
        legends = (QBF / "config/boards/shields/qbf_paintbrush/widgets/legends.h").read_text()
        self.assertIn('[QBF_LAYER_BASE] = {"BASE", "7321", "R^<B", "4568", "S>vL"}', legends)
        for half, row in (("left", "7321"), ("right", "4568")):
            self.assertIn(row, (QBF / "README.md").read_text(), half)


if __name__ == "__main__":
    unittest.main(verbosity=2)

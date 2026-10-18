#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Production schemas + build assertions using the pinned Zephyr DT toolchain.

Requires PyYAML and the existing workspace; no hardware or behavior mocks.
ZEPHYR_BASE and ZMK_APP may override the default build/baseline paths.
"""
import os
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
ZEPHYR = Path(os.environ.get("ZEPHYR_BASE", ROOT / "build/baseline/zephyr")).resolve()
ZMK = Path(os.environ.get("ZMK_APP", ROOT / "build/baseline/zmk/app")).resolve()
CC = shlex.split(os.environ.get("CC", "cc"))


class DevicetreeContract(unittest.TestCase):
    def generate(self, tmp, bindings="&bc 1 &bc 8 &bspc &bmode 2 &bsettings", extra=""):
        tmp = Path(tmp)
        source = tmp / "test.dts"
        source.write_text('''/dts-v1/;
#include <behaviors/braille.dtsi>
/* Including twice must not duplicate nodes. */
#include <behaviors/braille.dtsi>
/ {
    keymap {
        compatible = "zmk,keymap";
        base { bindings = <''' + bindings + '''>; };
    };
};
''' + extra)
        includes = [ROOT / "dts", ZMK / "dts", ZEPHYR / "dts", ZEPHYR / "include"]
        preprocessed = tmp / "test.pre.dts"
        with preprocessed.open("w") as output:
            subprocess.run(CC + ["-E", "-P", "-x", "assembler-with-cpp", "-nostdinc", "-undef"]
                           + [arg for inc in includes for arg in ("-I", str(inc))]
                           + [str(source)], stdout=output, check=True)
        (tmp / "include/generated").mkdir(parents=True)
        return subprocess.run([
            sys.executable, str(ZEPHYR / "scripts/dts/gen_defines.py"),
            "--dts", str(preprocessed), "--dtc-flags", "",
            "--bindings-dirs", str(ROOT / "dts/bindings"), str(ZMK / "dts/bindings"),
            str(ZEPHYR / "dts/bindings"),
            "--header-out", str(tmp / "include/generated/devicetree_generated.h"),
            "--dts-out", str(tmp / "resolved.dts"),
            "--edt-pickle-out", str(tmp / "edt.pickle"),
        ], capture_output=True, text=True)

    def compile(self, tmp):
        return subprocess.run(CC + [
            "-std=c11", "-Wall", "-Wextra", "-Werror", "-DCONFIG_ARCH_POSIX=1",
            "-I", str(Path(tmp) / "include/generated"), "-I", str(ZEPHYR / "include"),
            "-c", str(ROOT / "src/braille_bindings.c"), "-o", str(Path(tmp) / "bindings.o"),
        ], capture_output=True, text=True)

    def test_valid_bindings_and_default_mode(self):
        with tempfile.TemporaryDirectory() as tmp:
            bindings = " ".join(f"&bc {dot}" for dot in range(1, 9)) + " &bspc &bmode 2 &bsettings"
            result = self.generate(tmp, bindings)
            self.assertEqual(result.returncode, 0, result.stderr)
            header = (Path(tmp) / "include/generated/devicetree_generated.h").read_text()
            self.assertIn('#define DT_N_S_braille_P_default_mode "ueb1"', header)
            for node in ("dot", "space", "mode", "settings"):
                self.assertIn(f"DT_N_S_behaviors_S_braille_{node}_COMPAT_MATCHES_zmk_behavior_braille_{node} 1", header)
            self.assertIn("DT_N_S_keymap_S_base_P_bindings_LEN 11", header)
            self.assertIn("DT_N_S_keymap_S_base_P_bindings_IDX_9_VAL_param1 2", header)
            self.assertNotIn("DT_N_S_keymap_S_base_P_bindings_IDX_8_VAL_param1", header)
            self.assertNotIn("DT_N_S_keymap_S_base_P_bindings_IDX_10_VAL_param1", header)
            result = self.compile(tmp)
            self.assertEqual(result.returncode, 0, result.stderr)

    def test_named_mode_override_is_extensible(self):
        for mode in ("computer8", "ueb2", "future-japanese"):
            with self.subTest(mode=mode), tempfile.TemporaryDirectory() as tmp:
                result = self.generate(tmp, extra=f'&braille {{ default-mode = "{mode}"; }};')
                self.assertEqual(result.returncode, 0, result.stderr)
                header = (Path(tmp) / "include/generated/devicetree_generated.h").read_text()
                self.assertIn(f'#define DT_N_S_braille_P_default_mode "{mode}"', header)

    def test_invalid_dot_arguments_fail_compilation(self):
        for dot in (0, 9, 0xFFFFFFFF):
            with self.subTest(dot=dot), tempfile.TemporaryDirectory() as tmp:
                result = self.generate(tmp, f"&bc {dot}")
                self.assertEqual(result.returncode, 0, result.stderr)
                result = self.compile(tmp)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("Braille dot parameter must be in 1..8", result.stderr)

    def test_custom_devices_and_non_keymap_bindings(self):
        for dot in (1, 8, 9):
            with self.subTest(dot=dot), tempfile.TemporaryDirectory() as tmp:
                result = self.generate(tmp, extra='''
/ {
    behaviors {
        custom_dot: custom_dot {
            compatible = "zmk,behavior-braille-dot";
            #binding-cells = <1>;
        };
        test_macro {
            compatible = "zmk,behavior-macro";
            #binding-cells = <0>;
            bindings = <&custom_dot ''' + str(dot) + ''' &bsettings>;
        };
    };
};
''')
                self.assertEqual(result.returncode, 0, result.stderr)
                result = self.compile(tmp)
                if dot <= 8:
                    self.assertEqual(result.returncode, 0, result.stderr)
                else:
                    self.assertNotEqual(result.returncode, 0)
                    self.assertIn("Braille dot parameter must be in 1..8", result.stderr)

    def test_schema_and_cell_boundaries(self):
        invalid = [
            ("&bc", ""), ("&bmode", ""),
            ("&bc 1", "&bc { #binding-cells = <0>; };"),
            ("&bspc", "&bspc { #binding-cells = <1>; };"),
            ("&bc 1", "&bmode { #binding-cells = <0>; };"),
            ("&bc 1", "&bsettings { #binding-cells = <1>; };"),
            ("&bc 1", "&braille { default-mode = <1>; };"),
            ("&bc 1", "&bc { timeout-ms = <10>; };"),
            ("&bc 1", "&bspc { timeout-ms = <10>; };"),
            ("&bc 1", "&bmode { chord-timeout-ms = <10>; };"),
            ("&bc 1", "&braille { chord-timeout-ms = <10>; };"),
        ]
        for bindings, extra in invalid:
            with self.subTest(bindings=bindings, extra=extra), tempfile.TemporaryDirectory() as tmp:
                result = self.generate(tmp, bindings, extra)
                self.assertNotEqual(result.returncode, 0, result.stderr)
                self.assertIn("error", result.stderr.lower())

    def test_no_braille_nodes(self):
        with tempfile.TemporaryDirectory() as tmp:
            # Remove the test keymap too, then compile the production assertions.
            result = self.generate(tmp, "&bc 1", "/delete-node/ &bc; /delete-node/ &bspc;\n"
                                   "/delete-node/ &bmode; /delete-node/ &bsettings;\n"
                                   "/delete-node/ &braille; / { /delete-node/ keymap; };")
            self.assertEqual(result.returncode, 0, result.stderr)
            result = self.compile(tmp)
            self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main(verbosity=2)

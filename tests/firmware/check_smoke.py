#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Assert real firmware's resolved keymaps, feature gating, devices and UF2s.

Also imported by host tests against freshly parsed production keyboard keymaps.
Compile/link evidence only: this does not run a kernel or exercise hardware.
"""
import argparse
from pathlib import Path
import re


def defines(header):
    return dict(re.findall(r"^#define (\S+) (.+)$", header, re.MULTILINE))


def check_keymap(header, keyboard, mode="ueb1"):
    values = defines(header)

    def expect(key, value):
        assert values.get(key) == str(value), (key, values.get(key), value)

    def binding(layer, index, device, param=None):
        prefix = f"DT_N_S_keymap_S_{layer}_P_bindings_IDX_{index}"
        expect(prefix + "_PH", "DT_N_S_behaviors_S_" + device)
        if param is not None:
            expect(prefix + "_VAL_param1", param)

    expect("DT_N_S_braille_P_default_mode", f'"{mode}"')
    if keyboard == "qbf":
        expect("DT_N_S_keymap_S_default_layer_P_bindings_LEN", 16)
        for index, dot in enumerate((7, 3, 2, 1, 4, 5, 6, 8)):
            binding("default_layer", index, "braille_dot", dot)
        binding("default_layer", 12, "braille_space")
        binding("default_layer", 8, "momentary_layer", 1)
        binding("default_layer", 15, "momentary_layer", 2)
        for layer, index in (("default_layer", 0), ("raise_layer", 1), ("lower_layer", 2), ("alt_layer", 3)):
            expect(f"DT_N_S_keymap_S_{layer}_CHILD_IDX", index)
    else:
        for layer, index in (("base", 0), ("nav", 1), ("sym", 2), ("adj", 3), ("tap", 4), ("braille", 5)):
            expect(f"DT_N_S_keymap_S_{layer}_CHILD_IDX", index)
            expect(f"DT_N_S_keymap_S_{layer}_P_bindings_LEN", 42)
        # QWERTY home row stays conventional, including original hold-taps.
        for index, device in ((13, "key_press"), (14, "key_press"), (15, "key_press"),
                              (16, "homerow_layer_tap"), (19, "key_press"),
                              (20, "key_press"), (21, "key_press"), (22, "homerow_layer_tap")):
            binding("base", index, device)
        binding("base", 36, "braille_mode", 5)
        binding("base", 39, "key_press")  # ordinary Space is NOT Braille space
        binding("base", 41, "momentary_layer", 2)  # retained SYM access
        for index, dot in ((13, 7), (14, 3), (15, 2), (16, 1), (19, 4), (20, 5), (21, 6), (22, 8)):
            binding("braille", index, "braille_dot", dot)
        binding("braille", 39, "braille_space")
        for index in (11, 36):
            binding("braille", index, "to_layer", 0)
        # Every existing number/bracket combo is BASE-only, not a chord timeout.
        combo_layers = {key: value for key, value in values.items()
                        if re.match(r"DT_N_S_combos_S_combo_.*_P_layers_IDX_0$", key)}
        assert len(combo_layers) == 16, combo_layers
        for key, value in combo_layers.items():
            assert value == "0", (key, value)


def check_build(workspace, target):
    build = workspace / "build" / ("smoke-" + target)
    config = set((build / "zephyr/.config").read_text().splitlines())
    ninja = (build / "build.ninja").read_text()
    central = target in ("qbf-left", "qbf-computer8", "toucan-left", "toucan-computer8")
    assert ("CONFIG_ZMK_FEATURE_BRAILLE=y" in config) == central, target
    assert ("src/behavior_braille.c.obj:" in ninja) == central, target
    assert ("src/braille_bindings.c.obj:" in ninja) == central, target
    assert ("src/braille_settings.c.obj:" in ninja) == central, target
    assert "mode-settings/fixture.c" not in ninja and "routing/edge_spy.c" not in ninja, target
    assert "CONFIG_ZMK_BRAILLE_GRADE2=y" not in config, target
    assert "src/braille_rules_grade2.c.obj:" not in ninja, target
    assert (build / "zephyr/zmk.uf2").stat().st_size > 0, target
    assert (workspace / "logs" / f"smoke-{target}.exit").read_text().strip() == "0", target
    cache = (build / "CMakeCache.txt").read_text()
    # Module is discovered from west, not explicitly injected as an extra module.
    assert not re.search(r"^ZMK_EXTRA_MODULES:[^=]*=.*braille", cache, re.MULTILINE), target
    if not target.endswith("reset"):
        assert "CONFIG_ZMK_SPLIT=y" in config, target
        assert ("CONFIG_ZMK_SPLIT_ROLE_CENTRAL=y" in config) == central, target
        if target != "toucan-right":
            assert "CONFIG_ZMK_DISPLAY=y" in config, target
        if target.startswith("toucan"):
            assert "CONFIG_ZMK_POINTING=y" in config, target
            if central:
                assert "CONFIG_ZMK_STUDIO=y" in config, target
                assert "nice_view_gem" in ninja and "zmk-rgbled-widget" in ninja, target
            else:
                assert "input_pinnacle.c.obj:" in ninja, target
        header = (build / "zephyr/include/generated/devicetree_generated.h").read_text()
        check_keymap(header, target.split("-")[0], "computer8" if target.endswith("computer8") else "ueb1")
        if central:
            values = defines(header)
            link_map = (build / "zephyr/zmk.map").read_text()
            for symbol in ("braille_settings_enter", "braille_settings_exit",
                           "braille_input_dispatch", "braille_chord_discard",
                           "braille_rules_ueb1_characters", "braille_rules_computer8_characters",
                           "braille_tables_select"):
                assert symbol in link_map, (target, symbol)
            for device in ("dot", "space", "mode", "settings"):
                ordinal = values[f"DT_N_S_behaviors_S_braille_{device}_ORD"]
                assert f"._zmk_behavior_ref.static.zmk_behavior___device_dts_ord_{ordinal}_" in link_map, (target, device)
            # Factory data names these node-label devices even if the keymap
            # never uses them directly. Devicetree must retain and link both.
            for device in ("key_press", "sticky_key"):
                ordinal = values[f"DT_N_S_behaviors_S_{device}_ORD"]
                assert f"._zmk_behavior_ref.static.zmk_behavior___device_dts_ord_{ordinal}_" in link_map, (target, device)
            assert "CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_FULL=y" in config, target
    print(f"{target}: resolved layout/mode, module discovery, config, link and UF2 checks PASS")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--workspace", type=Path, default=Path("build/baseline"))
    parser.add_argument("targets", nargs="+")
    args = parser.parse_args()
    for target in args.targets:
        check_build(args.workspace, target)

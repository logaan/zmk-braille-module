---
created: Sun 18 Oct 2026 - 12h 59m 44s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: leaf-02-organize-the-out-of-tree-build
subject: Central-only out-of-tree scaffolding verified
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: gpt-6.1-sol
    name: leaf-02-organize-the-out-of-tree-build
---

# Out-of-tree build scaffolding delivered

## 1. Interfaces and changes

1. Completed only this leaf; implementation commit is `c8d1510` (`Replace hello-world stub with central Braille build scaffolding`).
2. Removed `src/braille.c` and its startup initializer/log message.
3. `CMakeLists.txt` builds the thin driver, reserved shared engine, and separate Grade 1/navigation/computer8 rule-data translation units only under `CONFIG_ZMK_FEATURE_BRAILLE`.
    1. `include/braille_rules.h` contains only the common rule forward declaration; the actual schema remains with its assigned leaf. No second matcher or character/output representation was introduced.
    2. `CONFIG_ZMK_BRAILLE_GRADE2` independently selects the optional Grade 2 translation unit, defaults off, and requires the feature. All engine/data translation units are explicitly empty implementation slots, not working tables or matching.
    3. ZMK v0.3 keeps `drivers/behavior.h` private to its app; the library explicitly includes `${APPLICATION_SOURCE_DIR}/include`. An initial real build caught this missing path; the corrected real builds pass.
4. `zephyr/module.yml` advertises this module's DTS root for the next leaf. Production bindings/default nodes were intentionally not added here.
5. `src/behavior_braille.c` registers every enabled instance of these compatibles through ZMK's `BEHAVIOR_DT_DEFINE` API:
    1. `zmk,behavior-braille-dot`.
    2. `zmk,behavior-braille-space`.
    3. `zmk,behavior-braille-mode`.
    4. `zmk,behavior-braille-settings`.
6. All devices explicitly use `BEHAVIOR_LOCALITY_CENTRAL`. Both callbacks return `-ENOTSUP`; they do not capture keys, switch layers, enter settings, match rules, or emit output. Default labels, parameter schemas/validation, and default mode selection remain with the binding leaf.
7. `README.md` documents the source boundaries, placeholder limits, manifest inclusion, release-correct `ZMK_EXTRA_MODULES` usage, central-only enablement, and host checks.
8. Added focused host C registration/unsupported-callback tests, CMake source-selection tests, and actual-parser Kconfig tests under `tests/host/`.

## 2. Host and prior regression checks

1. All commands below exited 0:

    ```sh
    python3 tests/host/test_build.py
    PYTHONPATH=build/baseline/zephyr/scripts/kconfig python3 tests/host/test_kconfig.py
    CC='cc -fsanitize=address,undefined -fno-omit-frame-pointer' python3 tests/host/test_build.py
    git diff --check
    LVGL="$PWD/build/baseline/modules/lib/gui/lvgl" ../qbf-paintbrushes/basic-zmk/screen-preview/build.sh left --physical
    LVGL="$PWD/build/baseline/modules/lib/gui/lvgl" ../qbf-paintbrushes/basic-zmk/screen-preview/build.sh right --physical
    ```

2. `test_build.py`: 2 tests pass, including all four feature/Grade-2 combinations, C compilation with five mock devices and with no nodes, central locality, and unsupported press/release results.
3. `test_kconfig.py`: 2 tests pass, including 16 split/central/feature/Grade-2 combinations and disabled defaults, with no Kconfig parser warnings.
4. Sanitized C harness: both tests pass with no ASan/UBSan diagnostics. The small host header shim is not Zephyr and does not prove real hardware behavior.
5. Both existing Paintbrush OLED preview checks pass. Logs: `build/scaffold/logs/preview-left.log`, `preview-right.log`.
6. Existing `tests/braille/**/*.yaml` are future conformance fixtures without an executable engine runner; no alphabet or literary conformance result is claimed.

## 3. Exact firmware build commands and results

1. Reused the baseline workspace, pinned ZMK v0.3.0, and `zmkfirmware/zmk-build-arm:3.5`; no second dependency workspace, core patches, or dependency edits.
2. The test-only overlay at `build/scaffold/devices.overlay` contains these nodes, deliberately without production binding schemas, binding cells, labels, or keymap references:

    ```dts
    / {
        behaviors {
            scaffold_dot { compatible = "zmk,behavior-braille-dot"; };
            scaffold_dot_2 { compatible = "zmk,behavior-braille-dot"; };
            scaffold_space { compatible = "zmk,behavior-braille-space"; };
            scaffold_mode { compatible = "zmk,behavior-braille-mode"; };
            scaffold_settings { compatible = "zmk,behavior-braille-settings"; };
        };
    };
    ```

3. These are the successful west command lines, grouped below in their identical Docker mounts for replay. Execution used background containers and per-target logs/exit files. The first target was initially configured pristine, then successfully rebuilt after the app-include correction; a final incremental rebuild also passed after the committed comment-only Grade-2 clarification.

    ```sh
    QBF=$(cd ../qbf-paintbrushes/basic-zmk && pwd)
    TOUCAN=$(cd ../zmk-keyboard-toucan && pwd)
    docker run --rm -v "$PWD/build/baseline:/work" -v "$PWD:/braille:ro" \
      -v "$QBF:/qbf:ro" -v "$TOUCAN:/toucan:ro" -w /work \
      zmkfirmware/zmk-build-arm:3.5 bash -ceuo pipefail '
        west zephyr-export
        west build -d build/scaffold-qbf-left -s zmk/app -b nice_nano_v2 -- \
          -DSHIELD=qbf_paintbrush_left -DZMK_CONFIG=/qbf/config \
          -DZMK_EXTRA_MODULES=/braille -DCONFIG_ZMK_FEATURE_BRAILLE=y \
          -DCONFIG_ZMK_BRAILLE_GRADE2=y \
          -DEXTRA_DTC_OVERLAY_FILE=/braille/build/scaffold/devices.overlay
        west build -p always -d build/scaffold-toucan-left -s zmk/app \
          -b seeeduino_xiao_ble -S studio-rpc-usb-uart -- \
          -DSHIELD="toucan_left rgbled_adapter nice_view_gem" \
          -DZMK_CONFIG=/toucan/config "-DZMK_EXTRA_MODULES=/toucan;/braille" \
          -DCONFIG_ZMK_STUDIO=y -DCONFIG_ZMK_FEATURE_BRAILLE=y
        west build -p always -d build/scaffold-qbf-right -s zmk/app -b nice_nano_v2 -- \
          -DSHIELD=qbf_paintbrush_right -DZMK_CONFIG=/qbf/config -DZMK_EXTRA_MODULES=/braille
        west build -p always -d build/scaffold-toucan-right -s zmk/app -b seeeduino_xiao_ble -- \
          -DSHIELD="toucan_right rgbled_adapter" -DZMK_CONFIG=/toucan/config \
          "-DZMK_EXTRA_MODULES=/toucan;/braille"
        west build -p always -d build/scaffold-qbf-disabled -s zmk/app -b nice_nano_v2 -- \
          -DSHIELD=qbf_paintbrush_left -DZMK_CONFIG=/qbf/config -DZMK_EXTRA_MODULES=/braille
      '
    ```

4. All five targets exited 0 and produced `build/baseline/build/<target>/zephyr/zmk.uf2`:
    1. `scaffold-qbf-left`: feature on, Grade-2 slot on, five real registered devices; FLASH 283752 B, RAM 64756 B.
    2. `scaffold-toucan-left`: feature on, Grade-2 off, no Braille nodes; FLASH 344976 B, RAM 155314 B. Studio, nice_view_gem, RGB adapter, and pointing remain enabled.
    3. `scaffold-qbf-right`: module discovered, feature off on peripheral; FLASH 245152 B, RAM 50476 B. Existing OLED remains enabled.
    4. `scaffold-toucan-right`: module discovered, feature off on peripheral; FLASH 185444 B, RAM 38148 B. Existing Cirque remains enabled.
    5. `scaffold-qbf-disabled`: module discovered, feature off on central; FLASH 283468 B, RAM 64748 B, matching baseline sizes.
5. Logs and exit codes: `build/baseline/logs/scaffold-*.log`, `scaffold-*.exit`; final central incremental log: `build/scaffold/logs/qbf-left-final-rebuild.log`.
6. Post-build assertions below exited 0; all five UF2/config/source-gating checks and all five linked behavior-reference checks passed:

    ```sh
    python3 - <<'PY'
    from pathlib import Path
    import re
    root = Path('build/baseline')
    for name, enabled, grade2 in [('qbf-left', True, True), ('toucan-left', True, False), ('qbf-right', False, False), ('toucan-right', False, False), ('qbf-disabled', False, False)]:
        build = root / 'build' / ('scaffold-' + name)
        config = (build / 'zephyr/.config').read_text().splitlines()
        ninja = (build / 'build.ninja').read_text()
        assert ('CONFIG_ZMK_FEATURE_BRAILLE=y' in config) == enabled
        assert ('CONFIG_ZMK_BRAILLE_GRADE2=y' in config) == grade2
        assert ('src/behavior_braille.c.obj:' in ninja) == enabled
        assert ('src/braille_rules_grade2.c.obj:' in ninja) == grade2
        assert 'CONFIG_ZMK_SPLIT=y' in config
        assert 'CONFIG_ZMK_DISPLAY=y' in config if name != 'toucan-right' else 'CONFIG_ZMK_POINTING=y' in config
        if name == 'toucan-left':
            assert 'CONFIG_ZMK_POINTING=y' in config and 'CONFIG_ZMK_STUDIO=y' in config
        assert (build / 'zephyr/zmk.uf2').stat().st_size > 0
        assert (root / 'logs' / ('scaffold-' + name + '.exit')).read_text().strip() == '0'
        print(name, 'config/source gating and UF2: PASS')
    build = root / 'build/scaffold-qbf-left/zephyr'
    header = (build / 'include/generated/devicetree_generated.h').read_text()
    link_map = (build / 'zmk.map').read_text()
    for node in ('dot', 'dot_2', 'space', 'mode', 'settings'):
        ordinal = re.search(r'#define DT_N_S_behaviors_S_scaffold_' + node + r'_ORD\s+(\d+)', header).group(1)
        assert '._zmk_behavior_ref.static.zmk_behavior___device_dts_ord_' + ordinal + '_' in link_map
        print('device', node, 'linked ZMK behavior reference: PASS')
    PY
    ```

## 4. Caveats and review boundary

1. No hardware was flashed or exercised. Test nodes establish real compile/link registration, not keymap parameter validation, runtime input, split transport correctness, or hardware behavior.
2. No production bindings/default nodes, input routing, chord detector, rule schema/matcher, alphabet data, settings implementation, or CI workflow was implemented preemptively. Full binding/keymap smoke coverage remains with the assigned later leaves.
3. Existing release/configuration warnings remain visible, including deprecated Nordic reboot storage, peripheral USB dependencies, and Toucan scroll-default macro redefinition; none is hidden by disabling baseline features.
4. ZMK, Zephyr, Cirque, and RGB-widget checkouts are clean. No sibling repository changes were made; unrelated `AGENTS.md` edits remain untouched.
5. Rechecked this task immediately before delivery; no newer coordinator messages were present.
6. No blockers remain. Delivered for coordinator review, not marked ready or done; waiting without starting another leaf.

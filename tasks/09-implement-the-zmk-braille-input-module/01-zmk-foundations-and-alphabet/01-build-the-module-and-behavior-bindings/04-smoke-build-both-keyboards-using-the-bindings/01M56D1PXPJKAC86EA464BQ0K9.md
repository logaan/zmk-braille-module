---
created: Sun 18 Oct 2026 - 13h 21m 58s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: leaf-04-smoke-build-both-keyboards-using-the-bindings
subject: Both-keyboard manifest smoke builds verified
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: gpt-6.1-sol
    name: leaf-04-smoke-build-both-keyboards-using-the-bindings
---

# Both-keyboard smoke builds delivered

## 1. Interfaces, changes, and commits

1. Completed only this leaf. All three implementation commits are pushed to their existing `main` branches.
    1. Module: `d70fee9a41408ddda5fe4aa14b29b34f16e75bc3` adds `scripts/smoke-build`, generated-artifact assertions, actual-keymap parser tests, the test-only named-mode overlay, and README documentation.
    2. Paintbrush: `1cbbcf9b0f86d537be4c4eb6a435b8081bba02fe` adds the pinned module manifest project, central-only enable config, Perkins BASE bindings, matching OLED legends, and documentation.
    3. Toucan: `46125e6e33d817df76b0ef91efbb5fae31f35891` adds the pinned module project, central-only enable option, BRL layer/access/return bindings, and documentation.
2. Both keyboard manifests reference published module revision `5273efebd5f13f7bff506609da79f5f9954b892e`, which contains the production schemas/default nodes.
    1. Published that revision before pushing dependent keyboard configs; GitHub API also resolves the exact SHA.
    2. The newer module commit adds only smoke tooling/tests/docs; production C/bindings remain the same as the referenced revision.
3. Both keymaps include `<behaviors/braille.dtsi>` and use physical `f d s` = dots 1 2 3, `j k l` = dots 4 5 6, `a` = dot 7, `;` = dot 8.
    1. QBF BASE top row is `7321 4568`; the key under `j` is `&bspc`. Existing Raise/Lower/Alt, arrows, Backspace, BT/DFU access, both OLEDs, and rotation code remain intact. Hand-maintained legends and README match.
    2. Toucan preserves QWERTY/NAV/SYM/ADJ/TAP at indices 0–4 and appends BRL at index 5. The duplicate left outer SYM thumb becomes `&bmode 5`; right outer SYM access remains. BRL uses the ordinary Space position for `&bspc`; its left outer thumb and top-right key explicitly return via `&to 0`. Existing number/bracket combos remain BASE-only.
4. Both production keyboards retain the schema default `ueb1`. A separate Toucan-central build applies `tests/firmware/computer8.overlay` and verifies the generated `computer8` name without selecting it in the production keymap.
5. Only the left central firmware enables `CONFIG_ZMK_FEATURE_BRAILLE=y`. Peripheral and reset images discover the module but compile no Braille sources; optional Grade 2 remains off.
6. `scripts/smoke-build` reuses the existing pinned `build/baseline` workspace and Docker 3.5, temporarily selects a generated combined manifest, and mounts this checkout at its ordinary west project path.
    1. It verifies real manifest-based module discovery, not a Braille `ZMK_EXTRA_MODULES` bypass; only the separately mounted Toucan keyboard uses `ZMK_EXTRA_MODULES=/toucan`.
    2. Local builds intentionally substitute current source for the remote pinned project. Existing sibling CI fetches the published module revision normally.
    3. The script restores the prior west manifest selection on exit; `build/baseline/.west/config` was verified restored to `manifest`.
    4. Do not run simultaneous west/config operations in that workspace. No dependencies are updated or patched by the smoke script.

## 2. Exact focused and regression checks

1. These commands exited 0:

    ```sh
    build/bindings/venv/bin/python tests/host/test_keyboards.py
    python3 tests/host/test_build.py
    PYTHONPATH=build/baseline/zephyr/scripts/kconfig python3 tests/host/test_kconfig.py
    build/bindings/venv/bin/python tests/host/test_devicetree.py
    CC="cc -fsanitize=address,undefined -fno-omit-frame-pointer" python3 tests/host/test_build.py
    LVGL="$PWD/build/baseline/modules/lib/gui/lvgl" ../qbf-paintbrushes/basic-zmk/screen-preview/build.sh left --physical
    LVGL="$PWD/build/baseline/modules/lib/gui/lvgl" ../qbf-paintbrushes/basic-zmk/screen-preview/build.sh right --physical
    bash -n scripts/smoke-build
    git diff --check
    git -C ../qbf-paintbrushes diff --check
    git -C ../zmk-keyboard-toucan diff --check
    ./scripts/td message validate
    gh api repos/logaan/zmk-braille-module/git/commits/5273efebd5f13f7bff506609da79f5f9954b892e --jq .sha
    ```

2. New keyboard suite: 4 tests pass using the actual pinned Zephyr parser/header generator on both complete sibling keymaps, with no behavior stubs.
    1. Verifies dot positions, Braille versus conventional space, QBF layer indices and legends, Toucan preserved layer indices/QWERTY behaviors, mode-key target and explicit return paths, 16 BASE-only combos, schema default, named override, and matching immutable module/release pins.
3. Prior suites: build contract 2 pass; Kconfig 2 pass; devicetree 6 pass. Sanitized existing host C harness: 2 pass with no ASan/UBSan diagnostics.
4. Both existing QBF OLED previews exit 0, retaining rotation/readback checks while displaying the updated legends. Logs: `build/smoke/preview-left.log`, `preview-right.log`.
5. Host logs: `build/smoke/host-keyboards.log`, `host-build.log`, `host-kconfig.log`, `host-devicetree.log`, `host-build-sanitized.log`.
6. `./scripts/smoke-build invalid-target` rejects the target with expected exit 2 before starting Docker; log: `build/smoke/invalid-target.log`.
7. TaskDown validation before delivery: 94 messages checked, 0 invalid; the delivery message is validated separately after creation.

## 3. Exact real firmware builds and artifacts

1. Ran `./scripts/smoke-build` with no arguments; all seven real pristine west builds and post-build checks passed. Aggregate log: `build/smoke/build.log`.
2. The script used `zmkfirmware/zmk-build-arm:3.5`, ZMK `edf5c0814fd3ea202e43aad2d68fd32e882a518c` (v0.3.0), the existing release workspace, and these exact west commands inside its documented Docker mounts:

    ```sh
    west manifest --validate
    west list zmk-braille-module
    west zephyr-export
    west build -p always -d build/smoke-qbf-left -s zmk/app -b nice_nano_v2 -- -DSHIELD=qbf_paintbrush_left -DZMK_CONFIG=/qbf/config
    west build -p always -d build/smoke-qbf-right -s zmk/app -b nice_nano_v2 -- -DSHIELD=qbf_paintbrush_right -DZMK_CONFIG=/qbf/config
    west build -p always -d build/smoke-toucan-left -s zmk/app -b seeeduino_xiao_ble -S studio-rpc-usb-uart -- -DSHIELD="toucan_left rgbled_adapter nice_view_gem" -DZMK_CONFIG=/toucan/config -DZMK_EXTRA_MODULES=/toucan
    west build -p always -d build/smoke-toucan-right -s zmk/app -b seeeduino_xiao_ble -- -DSHIELD="toucan_right rgbled_adapter" -DZMK_CONFIG=/toucan/config -DZMK_EXTRA_MODULES=/toucan
    west build -p always -d build/smoke-toucan-computer8 -s zmk/app -b seeeduino_xiao_ble -S studio-rpc-usb-uart -- -DSHIELD="toucan_left rgbled_adapter nice_view_gem" -DZMK_CONFIG=/toucan/config -DZMK_EXTRA_MODULES=/toucan -DEXTRA_DTC_OVERLAY_FILE=/work/zmk-braille-module/tests/firmware/computer8.overlay
    west build -p always -d build/smoke-qbf-reset -s zmk/app -b nice_nano_v2 -- -DSHIELD=settings_reset -DZMK_CONFIG=/qbf/config
    west build -p always -d build/smoke-toucan-reset -s zmk/app -b seeeduino_xiao_ble -- -DSHIELD=settings_reset -DZMK_CONFIG=/toucan/config -DZMK_EXTRA_MODULES=/toucan
    ```

3. All targets exit 0 and produce nonempty `build/baseline/build/smoke-<target>/zephyr/zmk.uf2`:
    1. `qbf-left`: FLASH 283700 B, RAM 64756 B, UF2 567808 B.
    2. `qbf-right`: FLASH 245152 B, RAM 50476 B, UF2 490496 B.
    3. `toucan-left`: FLASH 346012 B, RAM 156050 B, UF2 692224 B.
    4. `toucan-right`: FLASH 185444 B, RAM 38148 B, UF2 371200 B.
    5. `toucan-computer8`: FLASH 346012 B, RAM 156050 B, UF2 692224 B.
    6. `qbf-reset`: FLASH 46172 B, RAM 11552 B, UF2 92672 B.
    7. `toucan-reset`: FLASH 49512 B, RAM 11640 B, UF2 99328 B.
4. Per-target exact commands, complete warnings, logs, and exit codes: `build/baseline/logs/smoke-<target>.log` and `.exit`; discovery evidence: `smoke-discovery.log`.
5. Generated configuration and link evidence is preserved beside each UF2 in `.config`, `include/generated/devicetree_generated.h`, `zephyr.dts`, and `zmk.map`; source-selection evidence is in each build directory `build.ninja`.
6. Rechecked all seven targets separately with this command; exit 0:

    ```sh
    python3 tests/firmware/check_smoke.py --workspace build/baseline qbf-left qbf-right toucan-left toucan-right toucan-computer8 qbf-reset toucan-reset
    ```

7. Assertions verify generated layouts/mode names, central-only enable/source gating, Grade-2 exclusion, all four linked production behavior devices on all three central builds, QBF displays on both halves, Toucan central display/Studio/pointing/RGB-widget, peripheral Cirque compilation, and nonempty UF2s.

## 4. Remote CI and caveats

1. Existing sibling CI was triggered by the authorized pushes; firmware jobs remain in progress at the last check, not claimed successful:
    1. QBF: https://github.com/logaan/qbf-paintbrushes/actions/runs/39089713901
    2. Toucan: https://github.com/logaan/zmk-keyboard-toucan/actions/runs/39089718947
    3. Toucan keymap drawing/PDF job already succeeded. All required local firmware builds pass independently of pending remote runs.
2. No hardware was flashed or exercised. Parser checks, host C tests, OLED previews, and real compile/link builds do not prove runtime routing, split transport, mode-key semantics, characters, display hardware, or host keystrokes.
3. Production callbacks still explicitly return `-ENOTSUP`; in particular, the Toucan mode key does not enter BRL yet. No capture, matcher/output representation, character tables, runtime mode selection, settings UI, or later leaf was implemented.
4. Existing release/config warnings remain visible: config/boards deprecation, Nordic reboot-storage deprecation, peripheral USB dependencies, Toucan scroll-default macro redefinition, and settings-reset empty-keymap warnings.
5. ZMK, Zephyr, Cirque, and RGB-widget working trees remain clean. Unrelated pre-existing module `AGENTS.md` edits and Paintbrush `.lot-vault/Screens/006.md` remain untouched and uncommitted by this worker; no legacy task store was read or used.
6. Rechecked the assigned TaskDown task immediately before delivery; no newer coordinator feedback was present. No blockers remain. Delivered for coordinator review, not marked done; waiting without starting another leaf.

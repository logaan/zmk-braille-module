---
created: Sun 18 Oct 2026 - 12h 49m 27s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: leaf-01-baseline
subject: Pinned release baseline verified on both keyboards
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: gpt-6.1-sol
    name: leaf-01-baseline
---

# Pinned baseline delivered

## 1. Changes and commits

1. Completed only this baseline leaf; no Braille behavior/module implementation or later leaf was started.
2. Module repository: `be7bb68cfc0ba56ade3cb11c0b736e96dade9950` documents the authoritative `west/west.yml` development/CI manifest and freezes the shared upstream Cirque snapshot.
    1. Initial worker/status claim: `2414b60`.
    2. Per coordinator answer `01M56B2EBX`, actual module build/CI wiring remains with the later build/smoke leaves; no new module CI workflow was added here.
3. Paintbrush repository: `07bff74e917c5ccec17b57d62fa8d8398bedcf92`, committed and pushed to `main` per its contribution notes.
    1. Pin `basic-zmk/config/west.yml` to ZMK v0.3.0.
    2. Use release board `nice_nano_v2` and build image `zmkfirmware/zmk-build-arm:3.5` in local/CI/matrix configuration.
    3. Minimally adapt keyboard-owned OLED drawing to LVGL 8 canvas APIs and the release `zmk_endpoints_selected()` API; adapt its existing host preview and documentation.
    4. Preserve both displays, rotation options, normal keymap, legends, layers, and wireless split features.
4. Toucan repository: `f6b163f232ed63aa91c380b47bd5c2389302513f`, committed and pushed to `main` after the user's explicit permission to push to their repositories.
    1. Explicit ZMK v0.3.0 manifest pin, matching reusable CI workflow tag, and immutable upstream Cirque snapshot.
    2. No Toucan keymap, nice_view_gem source, or community module source changes.
5. No ZMK core, Zephyr, LVGL, Cirque, or RGB widget source was patched or forked; all tested dependency checkouts are clean.

## 2. Release verification and pins

1. Verified on 2026-10-18 using both commands:

    ```sh
    curl -fsSL https://api.github.com/repos/zmkfirmware/zmk/releases/latest
    git ls-remote https://github.com/zmkfirmware/zmk refs/tags/v0.3.0
    ```

2. GitHub reports latest non-prerelease release `v0.3.0`, published `2025-08-01T22:44:33Z`; the release target and tag both resolve to `edf5c0814fd3ea202e43aad2d68fd32e882a518c`.
    1. Release URL: https://github.com/zmkfirmware/zmk/releases/tag/v0.3.0
    2. Kagi search was attempted but has no configured API key; the direct upstream API/tag checks above succeeded.
3. All three manifests explicitly use `revision: v0.3.0` for ZMK.
4. Resolved/tested dependencies:
    1. ZMK: `edf5c0814fd3ea202e43aad2d68fd32e882a518c`.
    2. Upstream release-imported Zephyr `v3.5.0+zmk-fixes`: `dacab4875df72109b96cc8977547a0dc04875bcd`.
    3. LVGL 8.3.7: `8a6a2d1d29d17d1e4bdc94c243c146a39d635fdd`.
    4. Cirque upstream `toucan` snapshot, now explicitly frozen in module/Toucan manifests: `effec100f0cd3a38e7fba3adfea394586cc84cab`.
    5. RGB widget `v0.3`: `8756cb7b8114069fa3c25c6f6c990f24988fceff`.
    6. nice_view_gem is keyboard-local; it is unchanged from Toucan's original `640fa75` and included in the committed Toucan snapshot above.
5. Local toolchain: `zmkfirmware/zmk-build-arm:3.5`, digest `sha256:0979a8de325e3d822e48bd8d1d9efaee2984ad3750253119f920a342f0ce5cde`, Zephyr SDK 0.16.3.

## 3. Exact local baseline build commands

1. Fresh shared workspace preparation from this repository's manifest:

    ```sh
    mkdir -p build/baseline/manifest
    cp west/west.yml build/baseline/manifest/west.yml
    docker run --rm -v "$PWD/build/baseline:/work" -w /work       zmkfirmware/zmk-build-arm:3.5 bash -ceuo pipefail '
        west init -l manifest
        west update --narrow --fetch-opt=--depth=1
        west zephyr-export
      '
    ```

2. Build containers mount only the generated workspace and keyboard repositories, not the Braille module source:

    ```sh
    QBF=$(cd ../qbf-paintbrushes/basic-zmk && pwd)
    TOUCAN=$(cd ../zmk-keyboard-toucan && pwd)
    docker run --rm -v "$PWD/build/baseline:/work"       -v "$QBF:/qbf:ro" -v "$TOUCAN:/toucan:ro" -w /work       zmkfirmware/zmk-build-arm:3.5 bash -ceuo pipefail '
        west zephyr-export
        for shield in qbf_paintbrush_left qbf_paintbrush_right settings_reset; do
          west build -p always -d "build/qbf-$shield" -s zmk/app -b nice_nano_v2             -- -DSHIELD="$shield" -DZMK_CONFIG=/qbf/config
        done
        west build -p always -d build/toucan-left -s zmk/app           -b seeeduino_xiao_ble -S studio-rpc-usb-uart --           -DSHIELD="toucan_left rgbled_adapter nice_view_gem"           -DZMK_CONFIG=/toucan/config -DZMK_EXTRA_MODULES=/toucan -DCONFIG_ZMK_STUDIO=y
        west build -p always -d build/toucan-right -s zmk/app           -b seeeduino_xiao_ble -- -DSHIELD="toucan_right rgbled_adapter"           -DZMK_CONFIG=/toucan/config -DZMK_EXTRA_MODULES=/toucan
        west build -p always -d build/toucan-reset -s zmk/app           -b seeeduino_xiao_ble -- -DSHIELD=settings_reset           -DZMK_CONFIG=/toucan/config -DZMK_EXTRA_MODULES=/toucan
      '
    ```

3. These are the exact final west command lines; execution used background Docker containers with per-target log redirection so each exit code could be inspected independently.
4. Every final target exited **0** and produced `zephyr/zmk.uf2`:
    1. Paintbrush left: FLASH 283468 B; RAM 64748 B.
    2. Paintbrush right: FLASH 245152 B; RAM 50476 B.
    3. Paintbrush settings_reset: FLASH 46172 B; RAM 11552 B.
    4. Toucan left with Studio/display/RGB adapter: FLASH 344976 B; RAM 155314 B.
    5. Toucan right with Cirque/RGB adapter: FLASH 185444 B; RAM 38148 B.
    6. Toucan settings_reset: FLASH 49512 B; RAM 11640 B.
5. Logs: `build/baseline/logs/qbf-*-final.log` and `build/baseline/logs/toucan-*.log`; artifacts: `build/baseline/build/<target>/zephyr/zmk.uf2`.
6. Generated-config/artifact assertions passed for all six targets: no `BRAILLE` configuration symbols, each UF2 exists, both Paintbrush custom displays enabled, and Toucan central display/pointing/Studio enabled.
    1. Toucan left log confirms RGB widget and nice_view_gem compilation; right log confirms `cirque-input-module/drivers/input/input_pinnacle.c` compilation.
7. `west manifest --validate` passed; Python/PyYAML assertions passed for the matching ZMK and community pins in all three source manifests.
8. `bash -n` passed for both Paintbrush build scripts; `git diff --check` passed in all three affected repositories.

## 4. OLED compatibility regression checks

1. Initial Paintbrush builds failed on keyboard-owned LVGL 9 APIs (`lv_layer_t`, `LV_COLOR_FORMAT_L8`) absent from release LVGL 8.3.7; a subsequent central link failure exposed the renamed endpoint API. Both issues are resolved in the owned-code port, not hidden by disabling displays or replacing dependencies.
2. Existing preview commands against the tested release LVGL checkout both exited **0**:

    ```sh
    LVGL="$PWD/build/baseline/modules/lib/gui/lvgl"       ../qbf-paintbrushes/basic-zmk/screen-preview/build.sh left --physical
    LVGL="$PWD/build/baseline/modules/lib/gui/lvgl"       ../qbf-paintbrushes/basic-zmk/screen-preview/build.sh right --physical
    ```

3. Four left frames and two right frames match the saved pre-port preview output byte-for-byte; default and reverse rotation each pass pixel-exact readback checks for all six frames.
4. Both rotation variants were also compiled/run under AddressSanitizer and UndefinedBehaviorSanitizer, with **0** exits and zero compiler diagnostics:

    ```sh
    LVGL="$PWD/build/baseline/modules/lib/gui/lvgl"
    PREVIEW=$(cd ../qbf-paintbrushes/basic-zmk/screen-preview && pwd)
    WIDGETS=$(cd ../qbf-paintbrushes/basic-zmk/config/boards/shields/qbf_paintbrush/widgets && pwd)
    for mode in default ccw; do
      flags=()
      [ "$mode" = ccw ] && flags=(-DCONFIG_QBF_PAINTBRUSH_SCREEN_ROTATE_CCW=1)
      cc -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer         "${flags[@]}" -DLV_CONF_PATH=lv_conf.h         -I "$PREVIEW" -I "$LVGL" -I "$WIDGETS"         -o "build/baseline/preview-$mode" "$PREVIEW/main.c" "$WIDGETS/screen.c"         $(find "$LVGL/src" -name '*.c') -lm
      for half in left right; do
        "build/baseline/preview-$mode" "$half" --physical
      done
    done
    ```

5. The existing preview now exits nonzero on rotation mismatches; no handoff/progress files were added.

## 5. Remote verification and caveats

1. Paintbrush CI completed **successfully**, including all three firmware targets, artifact merge, and release publication: https://github.com/logaan/qbf-paintbrushes/actions/runs/39087569886
2. Toucan CI was triggered after the authorized push and was still **in progress** at the last check: https://github.com/logaan/zmk-keyboard-toucan/actions/runs/39087656508
    1. All required local Toucan firmware builds already pass; this delivery does not claim the pending remote result.
3. Non-fatal release/existing-config warnings remain visible in logs: deprecated `NRF_STORE_REBOOT_TYPE_GPREGRET`, peripheral `ZMK_USB=y` resolving to disabled by split-role dependencies, Toucan's existing scroll-default macro redefinition, and upstream settings_reset zero-keymap array warnings.
4. The upstream release manifest itself imports Zephyr through `v3.5.0+zmk-fixes`; the exact tested resolved revision is recorded above rather than silently modifying upstream dependencies.
5. No hardware was flashed or exercised; hardware acceptance remains the user-owned alphabet checkpoint.
6. No blockers remain for this leaf. Delivered for coordinator review; not marked done.

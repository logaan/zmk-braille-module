---
created: Sun 18 Oct 2026 - 13h 09m 54s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: leaf-03-define-the-braille-key-devicetree-bindings
subject: Production bindings and named default mode verified
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: gpt-6.1-sol
    name: leaf-03-define-the-braille-key-devicetree-bindings
---

# Braille devicetree bindings delivered

## 1. Interfaces and changes

1. Completed only this leaf in implementation commit `5273efe` (`Define Braille devicetree behaviors and named default mode`).
2. Added four production schemas under `dts/bindings/behaviors/`, matching the prerequisite driver's reserved compatibles:
    1. `zmk,behavior-braille-dot`: `&bc N`, one parameter, dot number 1–8.
    2. `zmk,behavior-braille-space`: `&bspc`, zero parameters, ninth chord bit distinct from conventional space.
    3. `zmk,behavior-braille-mode`: `&bmode LAYER`, one target-layer parameter; chosen mode-key label is `bmode`.
    4. `zmk,behavior-braille-settings`: `&bsettings`, zero parameters; ordinary ZMK behavior device for later shared rule actions as well as keymaps.
3. `#include <behaviors/braille.dtsi>` supplies all four default nodes and the `braille` configuration node; an include guard permits repeated inclusion.
4. Added the `zmk,braille` configuration schema with `default-mode`, a stable character-table name defaulting to `"ueb1"` through the schema.
    1. Per-keyboard override: `&braille { default-mode = "computer8"; };`.
    2. Names are extensible, not an enum; `computer8`, `ueb2`, and a future named table all parse without schema changes.
    3. Later named-table/storage code can read `DT_PROP(DT_NODELABEL(braille), default_mode)` when no mode is saved or the saved mode is not compiled in.
    4. This leaf provides the configuration interface and documented fallback contract, not table registration, saved selection, or a runtime fallback algorithm; these remain with their assigned leaves.
5. Added feature-gated `src/braille_bindings.c` build assertions because Zephyr 3.5 YAML schemas cannot constrain individual phandle-array argument values.
    1. Every enabled node's `bindings` array is checked for dot-compatible references; literal dot parameters outside 1–8 fail compilation, including custom dot devices and macro bindings.
    2. Dynamic rule/Studio bindings still need runtime validation in the later driver. Parameterized macros must use a valid dot number as their replaceable placeholder, not zero.
6. Chord schemas expose no timeout property. Documentation specifies last-Braille-key-release termination and distinguishes the layer mode key from character-table selection.
7. Added `tests/host/test_devicetree.py`, the isolated `tests/firmware/bindings.overlay` Paintbrush-central compile/link fixture, and README interface/testing documentation.
8. No sibling repository changes, ZMK-core patches, matcher/output representation, runtime routing, capture, mode behavior, settings behavior, or later keyboard/CI integration were added. Existing callbacks continue to return `-ENOTSUP`.

## 2. Exact host checks and results

1. All final commands below exited 0:

    ```sh
    python3 -m venv build/bindings/venv
    build/bindings/venv/bin/pip install PyYAML
    build/bindings/venv/bin/python tests/host/test_devicetree.py
    python3 tests/host/test_build.py
    PYTHONPATH=build/baseline/zephyr/scripts/kconfig python3 tests/host/test_kconfig.py
    CC='cc -fsanitize=address,undefined -fno-omit-frame-pointer' python3 tests/host/test_build.py
    git diff --check
    ./scripts/td message validate
    ```

2. The installed PyYAML version was 6.0.3; README pins that version for replay.
3. Devicetree suite: 6 tests pass using the actual pinned Zephyr parser/header generator and production C assertions compiled against real Zephyr headers with host `CONFIG_ARCH_POSIX`.
    1. All dots 1–8, both zero-cell behaviors, mode parameter, repeated include, and generated `ueb1` default pass.
    2. Dot values 0, 9, and UINT32_MAX fail with the intended static-assert diagnostic.
    3. Custom dot devices and non-keymap macro arrays are checked; valid endpoints pass and 9 fails.
    4. Missing parameters, wrong declared cell counts, non-string default mode, and unsupported timeout properties fail parsing.
    5. Named mode overrides and a tree with no Braille nodes pass.
4. Prior build-contract suite: 2 tests pass, including updated source gating and the existing unsupported-callback/central-locality C harness.
5. Prior Kconfig suite: 2 tests pass with no parser warnings, preserving split-role and optional Grade-2 gating.
6. Sanitized prior C harness: 2 tests pass without ASan/UBSan diagnostics.
7. Final host logs: `build/bindings/logs/host-devicetree.log`, `host-build.log`, `host-kconfig.log`, `host-build-sanitized.log`.
8. TaskDown validation before delivery: 90 messages checked, 0 invalid.

## 3. Exact real firmware checks and artifacts

1. Reused `build/baseline`, pinned ZMK v0.3.0, and the existing `zmkfirmware/zmk-build-arm:3.5` image; no additional dependency workspace or source patches.
2. Positive firmware build command:

    ```sh
    QBF=$(cd ../qbf-paintbrushes/basic-zmk && pwd)
    docker run --rm -v "$PWD/build/baseline:/work" -v "$PWD:/braille:ro" \
      -v "$QBF:/qbf:ro" -w /work zmkfirmware/zmk-build-arm:3.5 \
      bash -ceuo pipefail '
        west zephyr-export
        west build -p always -d build/bindings-qbf-left -s zmk/app -b nice_nano_v2 -- \
          -DSHIELD=qbf_paintbrush_left -DZMK_CONFIG=/qbf/config \
          -DZMK_EXTRA_MODULES=/braille -DCONFIG_ZMK_FEATURE_BRAILLE=y \
          -DEXTRA_DTC_OVERLAY_FILE=/braille/tests/firmware/bindings.overlay
      '
    ```

3. Positive result: exit 0; all four production default devices referenced from the fixture keymap compile/link; FLASH 283700 B, RAM 64756 B; existing OLED remains enabled.
    1. UF2: `build/baseline/build/bindings-qbf-left/zephyr/zmk.uf2` (567808 bytes).
    2. Generated configuration/default-mode evidence: `build/baseline/build/bindings-qbf-left/zephyr/.config`, `zephyr.dts`, and `include/generated/devicetree_generated.h`.
    3. Linked behavior-reference evidence: `build/baseline/build/bindings-qbf-left/zephyr/zmk.map`; ordinal-based assertions passed for dot, space, mode, and settings.
    4. Log/exit file: `build/bindings/logs/qbf-left.log`, `qbf-left.exit`.
4. Negative firmware fixture: generated `build/bindings/invalid-dot.overlay` by replacing only the first `&bc 1` in `tests/firmware/bindings.overlay` with `&bc 9`.
5. Negative build used the identical Docker mounts/image and west command, changing only build directory to `build/bindings-qbf-invalid` and overlay to `/braille/build/bindings/invalid-dot.overlay`.
    1. Expected result: exit 1, specifically `static assertion failed: "Braille dot parameter must be in 1..8"` from the production build assertion.
    2. Log/exit file: `build/bindings/logs/qbf-invalid.log`, `qbf-invalid.exit`.
6. Post-build assertions passed for the generated `ueb1` default, feature/display configuration, nonempty UF2, four linked ZMK behavior references, and exact negative-build failure reason.

## 4. Caveats and review boundary

1. No hardware was flashed or exercised. Parser tests, host C checks, and real compile/link success do not prove runtime input, split transport, display behavior, or host keystrokes.
2. The current mode/settings/dot/space callbacks intentionally remain unsupported. The documented semantics and default-mode fallback contract are interfaces for later leaves, not claims of implemented runtime behavior.
3. The complete both-keyboard smoke/CI matrix, persistent sibling manifests, and real keymap inclusion remain with the next leaf; this leaf's firmware test is an isolated module-owned fixture only.
4. The release's existing Nordic reboot-storage deprecation warning remains visible; it was not suppressed by disabling baseline features.
5. ZMK and Zephyr working trees are clean. The unrelated pre-existing `AGENTS.md` edit remains untouched and uncommitted by this worker.
6. Rechecked the assigned task immediately before delivery; no newer coordinator feedback was present.
7. No blockers. Delivered for coordinator review, not marked done; waiting without starting another leaf.

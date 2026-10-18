---
created: Sun 18 Oct 2026 - 13h 41m 34s
author:
  type: agent
  harness: pi
  model: moonshotai/kimi-k3
  name: leaf-05
subject: Thin behavior routing and release ownership verified
cargo:
  type: task
  status: delivered
  worker:
    type: agent
    harness: pi
    model: moonshotai/kimi-k3
    name: leaf-05
---

# Thin behavior routing delivered

## 1. Implementation and boundary

1. Completed only this leaf in commit `b67f45d` (`Route Braille behavior edges by binding parameter`).
2. `src/behavior_braille.c` now registers separate central-local dot/space APIs and forwards both press and release edges by binding parameter.
    1. `&bc N` maps dots 1–8 to `1 << (N - 1)`, mask values `0x001` through `0x080`.
    2. `&bspc` maps to `0x100`, distinct from conventional `&kp SPACE`.
    3. No position/layer filter, Braille on/off state, global key listener, timer, character dispatch, command dispatch, or alternate output representation.
    4. The original ZMK binding event is preserved, including position, layer, timestamp, and split source when present.
    5. All enabled instances work, including custom dot devices; each compatible can be compiled independently.
3. Runtime validation rejects dot values outside 1–8 before shifting and rejects nonzero unused parameters with `-EINVAL`, without forwarding an edge.
4. Added optional ZMK metadata for dot range 1–8 and zero-parameter space, verified through actual ZMK validation APIs as well as host tests.
5. Added the explicit detector boundary in `include/braille_chord.h`:

    ```c
    int braille_chord_key_event(uint16_t bit, bool pressed,
                               struct zmk_behavior_binding_event event);
    ```

    1. The input is exactly one validated bit plus the unchanged event; return contract is opaque success 0 or negative errno.
    2. `src/braille_chord.c` is a deliberately unsupported weak placeholder returning `-ENOTSUP`. Replace its body/linkage in the next detector leaf; this delivery does not implement held-key tracking, accumulation, or finalization.
    3. Tests substitute an edge spy only. They do not pretend to be a finished detector.
    4. Mode/settings callbacks remain unsupported and never feed chord bits; their implementation remains with the assigned later leaf.
6. Releases use the pinned ZMK press-time layer-state routing, not a module-owned current-layer lookup or duplicate layer router.
7. Updated README/Kconfig status, source selection, and repeatable routing checks. No sibling keyboard, manifest, ZMK-core, or dependency edits.

## 2. Host verification

1. These commands exited 0:

    ```sh
    python3 tests/host/test_build.py
    python3 tests/host/test_routing.py
    CC="cc -fsanitize=address,undefined -fno-omit-frame-pointer" python3 tests/host/test_routing.py
    PYTHONPATH=build/baseline/zephyr/scripts/kconfig python3 tests/host/test_kconfig.py
    build/bindings/venv/bin/python tests/host/test_devicetree.py
    build/bindings/venv/bin/python tests/host/test_keyboards.py
    bash -n scripts/test-routing
    git diff --check
    ```

2. New routing suite: 2 tests pass; compiled/runs all 4 split/metadata combinations and all 16 compatible-presence combinations with metadata off/on.
    1. All eight dot parameters and space across 32 layers, seven positions including 0 and UINT32_MAX, and two dot devices.
    2. Exact bit, edge direction, event fields, unchanged bindings, central locality, metadata, and downstream error propagation.
    3. Invalid dot values 0, 9, 31, 32, 256, UINT32_MAX; unused dot/space parameters; no detector calls on rejection.
    4. Mode/settings exclusion and explicitly unsupported unavailable detector.
3. ASan/UBSan routing suite passes with no diagnostics.
4. Regression suites pass: build contract 2 tests, Kconfig 2, devicetree 6, actual sibling keymaps 4.
5. New/changed C files pass `clang-format --style=file:build/baseline/zmk/.clang-format --dry-run --Werror`.
6. Host logs: `build/routing/logs/host-{build,routing,routing-sanitized,kconfig,devicetree,keyboards}.log`.

## 3. Real ZMK runtime and firmware verification

1. `./scripts/test-routing` exits 0 with the pinned ZMK v0.3.0, Zephyr 3.5, and existing Docker image `zmkfirmware/zmk-build-arm:3.5`.
    1. Builds/runs `native_posix_64` using the real ZMK keymap, layer APIs, behavior invocation, mock scan driver, and Zephyr kernel; no source patches.
    2. Separate module `tests/firmware/routing/` supplies a strong edge spy and a non-consuming observer of conventional keycode events.
    3. All 61 reviewed snapshot events match: every dot on two layouts, a custom dot device, Braille space, conventional Space/A/B, ordinary typing while a dot is held, and transparent-layer fallback.
    4. Press-time release ownership passes when switching layers while holding a dot, custom-instance dot, Braille space, or conventional space. Releases neither change dot number nor convert between Braille and conventional input.
    5. Actual ZMK metadata validation accepts all valid dots/space and rejects invalid/unused parameters.
    6. The native test has no host transport; expected USB-endpoint-unsupported logs remain visible. Its result is keycode-event/routing evidence, not physical host typing.
2. Native exact west command, executed by the script inside the documented Docker mounts:

    ```sh
    west build -p always -d build/routing-native -s zmk/app -b native_posix_64 -- \
      -DZMK_CONFIG=/braille/tests/firmware/routing \
      "-DZMK_EXTRA_MODULES=/braille;/braille/tests/firmware/routing"
    timeout 30 build/routing-native/zephyr/zmk.exe
    ```

3. Native artifact: `build/baseline/build/routing-native/zephyr/zmk.exe`; logs: `build/baseline/logs/routing-{build,native,run}.log`.
4. `./scripts/smoke-build qbf-left toucan-left` exits 0; both pristine real central firmware builds and generated-artifact assertions pass with current production source and ordinary manifest discovery.
    1. QBF central: FLASH 284068 B, RAM 64756 B, UF2 568320 B.
    2. Toucan central: FLASH 346436 B, RAM 156050 B, UF2 693248 B; Studio/display/pointing preserved.
    3. Artifacts: `build/baseline/build/smoke-{qbf-left,toucan-left}/zephyr/zmk.uf2`; exact commands/logs/exits: `build/baseline/logs/smoke-{qbf-left,toucan-left}.{log,exit}`. Aggregate: `build/baseline/logs/routing-keyboards.log`.
    4. Link/source assertions confirm the production detector placeholder is present and test spy sources do not leak into either firmware.
    5. Rechecked both with `python3 tests/firmware/check_smoke.py --workspace build/baseline qbf-left toucan-left`; exit 0. West manifest selection is restored to `manifest`.
5. No hardware was flashed or exercised. Wireless split transport, complete chord capture, mode-key semantics, and host output remain unverified/unimplemented here as scoped.

## 4. Coordination and review

1. Read the coordinator progress check and replied with completed verification before delivery. Checked the task again immediately before this delivery; no newer instructions were present.
2. ZMK and Zephyr checkouts remain clean. Unrelated `AGENTS.md`, coordinator work, and coordinator-authored TaskDown messages were preserved and excluded from implementation commits.
3. No blockers. Delivered for coordinator review, not marked done; stopping without starting another leaf.

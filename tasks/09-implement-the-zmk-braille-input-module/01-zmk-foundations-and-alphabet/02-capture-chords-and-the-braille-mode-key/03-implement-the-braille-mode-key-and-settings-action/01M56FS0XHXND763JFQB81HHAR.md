---
created: Sun 18 Oct 2026 - 14h 09m 39s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: alphabet-leaf-07
subject: Mode layer composition and settings isolation verified
cargo:
  type: task
  status: delivered
---

# Mode key and settings action delivered

## 1. Implementation and boundaries

1. Completed only this assigned leaf in commit `3d84f3f` (`Compose Braille mode layers and isolate settings input`).
2. Preserved the one-parameter `&bmode LAYER` compatible/interface.
    1. Its central-local adapter passes the same target as both parameters of the internal ZMK hold-tap device, preserving the original event and returning downstream errors.
    2. Composes stock `hold-preferred`, `hold-while-undecided`, momentary-layer hold, and to-layer tap; 200 ms tapping term and no quick-tap window.
    3. Target is active immediately; any other key press resolves hold before that key is dispatched, allowing target-layer Braille chording even before the tapping term.
    4. Timer-resolved hold is momentary; tap switches to the target. Returning works through a target-layer `&bmode 0` or an ordinary layer key.
    5. No mode/layer edge enters a chord mask. The only timer belongs to stock ZMK hold-tap; chord capture remains timer-free.
    6. Runtime validation rejects nonexistent target layers and nonzero unused parameters; optional metadata exposes the layer parameter.
3. Implemented `&bsettings` as an ordinary central-local, zero-parameter ZMK action.
    1. Press enters settings; release is handled without exiting. Invalid unused parameters return `-EINVAL`.
    2. No factory chord is assigned here; later command research and overrides own that mapping.
4. Added `include/braille_settings.h` and `src/braille_settings.c`.
    1. `braille_settings_enter(event)`, `braille_settings_exit(event)`, and `braille_settings_active()` expose an idempotent serialized lifecycle.
    2. Both transitions discard current capture, invoke `braille_input_reset()` without committing output, then notify `braille_settings_changed(active, event)`.
    3. While settings is active, completed capture goes exclusively to `braille_settings_chord(mask, event)`, never the typing consumer; UI errors propagate without typing fallback.
    4. Added `braille_chord_discard()`: retain held-key ownership and drop the entire capture through the next idle point, including late joins. Neither entry-spanning nor exit-spanning input can leak or replay.
    5. Reset/UI hooks have weak defaults. There is no production pending buffer yet; the future shared engine must implement the reset hook to discard pending multi-chord input and reset context without output.
    6. The future ordered host-action dispatcher must check `braille_settings_active()` before output, including queued actions. These gates suspend module-generated Braille typing, not conventional keymap behavior or already-held host keys.
5. Updated source gating, README/Kconfig implementation status, host regressions, and smoke assertions for production settings linkage and exclusion of test fixtures.
6. No matcher, rule/action schema, character table, settings UI/storage, sibling keymap/manifest, ZMK-core, or dependency changes were made.

## 2. Host verification

1. All commands exited 0:

    ```sh
    python3 tests/host/test_build.py
    python3 tests/host/test_routing.py
    python3 tests/host/test_chord.py
    python3 tests/host/test_settings.py
    CC='cc -fsanitize=address,undefined -fno-omit-frame-pointer' python3 tests/host/test_build.py
    CC='cc -fsanitize=address,undefined -fno-omit-frame-pointer' python3 tests/host/test_routing.py
    CC='cc -fsanitize=address,undefined -fno-omit-frame-pointer' python3 tests/host/test_chord.py
    CC='cc -fsanitize=address,undefined -fno-omit-frame-pointer' python3 tests/host/test_settings.py
    PYTHONPATH=build/baseline/zephyr/scripts/kconfig python3 tests/host/test_kconfig.py
    build/bindings/venv/bin/python tests/host/test_devicetree.py
    build/bindings/venv/bin/python tests/host/test_keyboards.py
    bash -n scripts/test-mode-settings
    git diff --check
    ./scripts/td message validate
    ```

2. Settings suite compiles production capture and settings gates with fixture pending/UI consumers, split disabled and enabled.
    1. Unresolved prefix discarded on entry, reset-before-notification, idempotent entry/exit, settings-only routing, UI error propagation, no host dispatch or stale-prefix replay.
    2. Capture spanning entry, exit, or an entire settings session is discarded through idle; late joins are dropped and fresh typing resumes correctly afterward.
3. Build/routing/chord regressions pass, including all compatible-presence combinations, split/metadata combinations, forwarding/error/parameter validation, mode/settings metadata, and all 511 nonempty chord masks.
4. Kconfig 2 tests, devicetree 6, actual sibling-keymap 4 pass. All four sanitized suites pass without ASan/UBSan diagnostics.
5. Changed/new C production and test files pass release-style `clang-format --dry-run --Werror`.
6. Logs: `build/mode-settings/logs/host-*.log`.

## 3. Real pinned ZMK and firmware verification

1. New `./scripts/test-mode-settings` exits 0 on pinned ZMK v0.3.0 / Zephyr 3.5 in `zmkfirmware/zmk-build-arm:3.5`.
    1. Real ZMK keymap, mock scan events, hold-tap/layer behaviors, production capture/settings gates, and only test-owned pending/UI/action fixtures.
    2. All 12 assertion checkpoints pass: interrupted hold with overlapping Braille dots, timer hold with Braille space, tap-to-layer, mode-key return, ordinary-layer return, release routing when mode releases before the dot, and exact masks excluding mode.
    3. A fixture rule action invokes production `&bsettings` through the ordinary ZMK behavior API; tests pending reset, release-not-exit, settings-only input, entry/exit-spanning captures, explicit exit hook, and no host keycode events.
    4. Actual metadata validation and runtime invalid-parameter rejection pass. ZMK v0.3's layer metadata uses physical key count as its upper bound; the adapter independently checks the actual layer count. No core patch was needed.
2. Exact new native command inside the script's documented Docker mounts:

    ```sh
    west build -p always -d build/mode-settings-native -s zmk/app -b native_posix_64 -- \
      -DZMK_CONFIG=/braille/tests/firmware/mode-settings \
      '-DZMK_EXTRA_MODULES=/braille;/braille/tests/firmware/mode-settings'
    timeout 30 build/mode-settings-native/zephyr/zmk.exe
    ```

3. New native logs: `build/baseline/logs/mode-settings-{build,native}.log`; executable: `build/baseline/build/mode-settings-native/zephyr/zmk.exe`; aggregate: `build/mode-settings/logs/native.log`.
4. `./scripts/test-routing` also exits 0 on the final production source: all unchanged 81 routing/conventional-key events and 26 chord snapshots match. Aggregate log: `build/mode-settings/logs/routing.log`.
5. `./scripts/smoke-build qbf-left toucan-left` exits 0; both real pristine central builds and artifact checks pass.
    1. QBF: FLASH 286828 B, RAM 67396 B, UF2 573952 B; display retained.
    2. Toucan: FLASH 347284 B, RAM 156418 B, UF2 694784 B; Studio/display/pointing retained.
    3. Artifacts: `build/baseline/build/smoke-{qbf-left,toucan-left}/zephyr/zmk.uf2`; logs/exits: `build/baseline/logs/smoke-{qbf-left,toucan-left}.{log,exit}`; aggregate: `build/mode-settings/logs/keyboards.log`.
    4. Final `python3 tests/firmware/check_smoke.py --workspace build/baseline qbf-left toucan-left` passes, including production settings/discard/gate symbols and absence of test fixture sources.
6. No hardware was flashed. Native tests have no physical host transport; wireless split transport, physical keyboards, actual host typing, and later UI/output integration are not claimed.

## 4. Review boundary

1. Rechecked this task immediately before delivery; no newer task instructions were present.
2. Preserved unrelated `AGENTS.md` and coordinator-authored messages; excluded them from implementation commits. ZMK and Zephyr checkouts remain clean.
3. No blockers. Delivered for review, not marked done; stopping without starting another task.

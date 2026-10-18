---
created: Sun 18 Oct 2026 - 13h 52m 44s
author:
  type: agent
  harness: pi
  model: gpt-6.1-sol
  name: chord-finalization
subject: Nine-bit chord capture and last-release resolution verified
cargo:
  type: task
  status: delivered
---

# Nine-bit chord capture delivered

## 1. Implementation and interfaces

1. Completed only this leaf in commit `f7119a8` (`Capture nine-bit Braille chords until the last release`).
2. Replaced the weak detector placeholder with the production `braille_chord_key_event(bit, pressed, event)` implementation.
    1. First Braille press begins capture; every subsequent press while any Braille key remains held joins the accumulated mask.
    2. Early releases retain their accumulated bits; late joins and re-presses remain in the same chord.
    3. Exactly one resolution occurs on the last held key's release, then capture is idle.
    4. Dots 1–8 are bits 0–7; Braille space is bit 8 (`0x100`). Dot-only, space-only, and space-plus-dot masks follow the same path.
    5. No timer, layer filter, on/off state, character dispatch, command dispatch, or global key listener.
3. Held-key identity uses position plus split source, independently of layer and binding device.
    1. Distinct physical keys bound to the same dot must both release before resolution.
    2. Duplicate press/release notifications and unmatched releases do not create extra resolutions.
    3. Invalid/non-single-bit input and an inconsistent bit for an already-held key return `-EINVAL` without mutation.
    4. Storage is bounded by `ZMK_KEYMAP_LEN`; excess distinct presses return `-ENOSPC` without contaminating the chord.
    5. Calls must be serialized, matching ZMK's central behavior-event path; no concurrent-caller or split-disconnect recovery guarantee is added.
4. Added the synchronous completion boundary in `include/braille_chord.h`:

    ```c
    int braille_chord_resolved(uint16_t mask,
                              struct zmk_behavior_binding_event event);
    ```

    1. Receives the completed mask and unchanged last-release event exactly once.
    2. State is idle before dispatch, including when the consumer fails or reenters capture.
    3. Consumer errors propagate to the final releasing behavior; completed chords are not retried.
    4. Its weak production default returns `-ENOTSUP` until the shared engine supplies the consumer. Capture is implemented; matching/output remain explicitly unavailable.
5. Updated README/Kconfig and adapted existing registration checks to successful capture presses and unsupported final consumer releases.
6. Mode/settings behavior, common rule schema/matching/output, sibling keyboards, manifests, ZMK core, and dependencies were not changed.

## 2. Host verification

1. All commands exited 0:

    ```sh
    python3 tests/host/test_chord.py
    CC='cc -fsanitize=address,undefined -fno-omit-frame-pointer' python3 tests/host/test_chord.py
    python3 tests/host/test_build.py
    python3 tests/host/test_routing.py
    CC='cc -fsanitize=address,undefined -fno-omit-frame-pointer' python3 tests/host/test_routing.py
    PYTHONPATH=build/baseline/zephyr/scripts/kconfig python3 tests/host/test_kconfig.py
    build/bindings/venv/bin/python tests/host/test_devicetree.py
    build/bindings/venv/bin/python tests/host/test_keyboards.py
    bash -n scripts/test-routing
    clang-format --style=file:build/baseline/zmk/.clang-format --dry-run --Werror src/braille_chord.c include/braille_chord.h tests/host/test_chord.c tests/firmware/routing/edge_spy.c
    git diff --check
    ./scripts/td message validate
    ```

2. New detector suite compiles/runs the production detector with only a completion spy, split disabled and enabled.
    1. All 511 nonempty masks, each press/release checked for resolution count, including all nine bits and space-only versus combined space/dot masks.
    2. Overlap, early release, late join, re-press, consecutive chords, changed layer on release, and timestamp extremes demonstrating timer independence.
    3. Duplicate physical dot bindings, duplicate/unknown edges, arbitrary positions, and distinct split sources sharing a position.
    4. Invalid inputs, inconsistent bits, full capacity/overflow, consumer error propagation, and reentrant capture after completion.
3. ASan/UBSan detector and routing runs pass with no diagnostics.
4. Existing suites pass: build 2 tests, routing 2, Kconfig 2, devicetree 6, actual sibling keymaps 4.
5. Logs: `build/chords/logs/host-*.log`.

## 3. Real pinned ZMK and firmware verification

1. `./scripts/test-routing` exits 0 with pinned ZMK v0.3.0, Zephyr 3.5, and Docker `zmkfirmware/zmk-build-arm:3.5`.
    1. The native fixture no longer replaces the detector: a linker wrapper logs incoming edges and always calls the production detector; only the unavailable completion consumer is supplied by the test module.
    2. Real keymap/layer APIs, behavior dispatch, mock scanning, and Zephyr kernel produce exactly the reviewed 81 routing/conventional-key events and 26 completed chords.
    3. Includes all nine bits, ordinary Space/A/B exclusion, layer-change release ownership, overlapping presses, early releases, late joins, re-presses, duplicate dot bindings, and a chord spanning a layer switch with newly reached bindings joining it.
    4. Each completion immediately follows its last release and preserves that release's position/layer; real metadata validation continues to pass.
    5. No ZMK patches. Expected native USB-endpoint-unsupported messages remain visible; no host transport or physical typing is claimed.
2. Native commands remain reproducible through the script:

    ```sh
    west build -p always -d build/routing-native -s zmk/app -b native_posix_64 -- \
      -DZMK_CONFIG=/braille/tests/firmware/routing \
      '-DZMK_EXTRA_MODULES=/braille;/braille/tests/firmware/routing'
    timeout 30 build/routing-native/zephyr/zmk.exe
    ```

3. Native logs: `build/baseline/logs/routing-{build,native}.log`; aggregate: `build/chords/logs/native.log`; executable: `build/baseline/build/routing-native/zephyr/zmk.exe`.
4. `./scripts/smoke-build qbf-left toucan-left` exits 0; both pristine real central firmware builds and generated-artifact assertions pass.
    1. QBF central: FLASH 284260 B, RAM 64892 B, UF2 568832 B.
    2. Toucan central: FLASH 346628 B, RAM 156386 B, UF2 693760 B; Studio/display/pointing retained.
    3. Asserted production detector/completion symbols and source are linked; no test spy or linker wrapper leaks into either keyboard firmware.
    4. Artifacts: `build/baseline/build/smoke-{qbf-left,toucan-left}/zephyr/zmk.uf2`; exact commands/logs/exits: `build/baseline/logs/smoke-{qbf-left,toucan-left}.{log,exit}`; aggregate: `build/chords/logs/keyboards.log`.
5. No hardware was flashed or exercised. Wireless split transport, physical hardware, mode-key behavior, rule matching, and host output remain outside this leaf.

## 4. Review boundary

1. Preserved unrelated `AGENTS.md` and coordinator messages; excluded them from implementation commits. ZMK and Zephyr checkouts remain clean.
2. Rechecked this task before delivery; no newer instructions or blockers were present.
3. Delivered for review, not marked done. Stopping without starting another leaf.

---
created: Sun 18 Oct 2026 - 16h 46m 50s
author:
  type: agent
  harness: pi
  model: openrouter/pareto-code
  name: alphabet-leaf-12
subject: Alphabet hardware handoff prepared
cargo:
  type: task
  status: draft
---

# Alphabet hardware handoff

## 1. Firmware artifacts (fresh, built for this checkpoint)

1.  Built locally with `./scripts/smoke-build qbf-left qbf-right toucan-left
    toucan-right` at module commit `4ce0868` (`Map the 26 six-dot alphabet
    cells`), in `zmkfirmware/zmk-build-arm:3.5`; all four pristine builds
    exited 0 and `tests/firmware/check_smoke.py` passed on all four.
2.  Staged UF2s (copy these to the bootloader drives):

    | Target | UF2 | SHA-256 |
    |--------|-----|---------|
    | QBF Paintbrush left (central) | `build/handoff/alphabet/qbf-paintbrush-left.uf2` | `41275351b284f7d9a46a874bc9300d55cdbe8eb271b01c9825b4f722d54a5e62` |
    | QBF Paintbrush right (peripheral) | `build/handoff/alphabet/qbf-paintbrush-right.uf2` | `20ca288515b11acf34f6ba6931dd6f365cb5651712c22ccfafe03f74f5e88a02` |
    | Toucan left (central) | `build/handoff/alphabet/toucan-left.uf2` | `c6e8a9cf7c248f09e0cd5e0997937ae7df5f2827bf4913bfb71ce3f353742f4a` |
    | Toucan right (peripheral) | `build/handoff/alphabet/toucan-right.uf2` | `286d9c71c03dc1b71b9e2f448a7473b607e4320d745233bfe39a719b1b2f0697` |

3.  Original build-tree paths: `build/baseline/build/smoke-<target>/zephyr/zmk.uf2`;
    per-target logs and exit codes: `build/baseline/logs/smoke-<target>.{log,exit}`.
4.  `CONFIG_ZMK_FEATURE_BRAILLE=y` is on for both central (left) builds only;
    both central maps link `braille_rules_ueb1_characters`,
    `braille_chord_resolved`, `braille_engine_commit_rule`, and the
    `braille_tables_boot` startup selection of the `ueb1` default mode.

## 2. Current published pins

1.  ZMK: v0.3.0, `edf5c0814fd3ea202e43aad2d68fd32e882a518c` (all three
    manifests pin `v0.3.0`).
2.  Zephyr: `v3.5.0+zmk-fixes`, `dacab4875df72109b96cc8977547a0dc04875bcd`.
3.  Keyboards: QBF Paintbrushes `1cbbcf9b0f86d537be4c4eb6a435b8081bba02fe`
    (main); Toucan `46125e6e33d817df76b0ef91efbb5fae31f35891` (main).
4.  Module: both keyboard manifests publish-pin `5273efebd5f13f7bff506609da79f5f9954b892e`;
    per the smoke-build script's documented behavior, these local builds
    substitute the current checkout (`4ce0868`) for that pinned project so
    the new alphabet table is included. Production C/bindings are unchanged
    between those revisions except this leaf's alphabet table, boot-time
    default-mode selection, and tests/docs.
5.  Build container: `zmkfirmware/zmk-build-arm:3.5`, digest
    `sha256:0979a8de325e3d822e48bd8d1d9efaee2984ad3750253119f920a342f0ce5cde`.

## 3. Flashing instructions (no hardware was flashed by the agent)

1.  Flash both halves of the keyboard under test; order does not matter.
    1.  QBF Paintbrushes (nice!nano v2): plug in one half, double-tap its
        reset button; a `NICENANO` USB drive mounts. Copy
        `qbf-paintbrush-left.uf2` to the left half's drive and
        `qbf-paintbrush-right.uf2` to the right half's drive; each drive
        ejects itself when done.
    2.  Toucan (seeeduino_xiao_ble): plug in one half, double-tap reset
        (two quick presses of the tiny reset button); a `XIAO-BLE` USB drive
        mounts. Copy `toucan-left.uf2` to the left half's drive and
        `toucan-right.uf2` to the right half's drive.
2.  After flashing, connect the left (central) half to the Mac by USB or
    Bluetooth as usual; the right half pairs to the left automatically.
3.  Reverting: reflash the previous UF2s the same way; no settings were
    cleared by these builds (settings_reset images were not part of this
    handoff).

## 4. Braille key layout for the checks

1.  Perkins mapping on both keyboards: physical `f d s` = dots 1 2 3,
    `j k l` = dots 4 5 6, `a` = dot 7, `;` = dot 8. Dots 7/8 have no alphabet
    effect; the alphabet uses dots 1–6 only.
2.  QBF Paintbrushes: Braille keys are on the BASE layer; `&bspc` is the key
    under `j` on the top row's lower row position per its legends.
3.  Toucan: the BRL layer is index 5. Hold the left outer thumb (`&bmode 5`)
    while chording, or tap it to switch and return with the BRL layer's
    `&to 0` keys (left outer thumb or top-right key). `&bspc` sits at the
    ordinary Space position on BRL.
4.  Default character mode is `ueb1`, selected at boot; no
    settings action is needed before typing.

## 5. Expected behavior boundaries for this checkpoint

1.  Each completed chord emits exactly one ordinary lowercase letter on the
    final dot release; holding a chord any length emits nothing until then.
2.  The mode's fixture command chords also emit: Braille space -> space,
    space+dots 1,5 -> Return, space+dots 4,5 -> Tab, space+dot 3 -> Left
    arrow, space+dot 6 -> Right arrow. These are fixtures pending phase-02
    research, not part of this checkpoint's acceptance.
3.  Six-dot cells outside the alphabet (e.g. dot 3 alone, dots 1–6) emit
    nothing by design until phase 03's literary signs.
4.  Capitals, numbers, punctuation, displays, settings, and persistence are
    out of scope here, matching this task's section 1.5.

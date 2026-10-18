---
created: Sun 18 Oct 2026 - 17h 06m 21s
author:
  type: agent
  harness: pi
  model: openai/gpt-6.1-sol
  name: sol
subject: 'Sol final check: ready for user alphabet hardware test'
cargo:
  type: task
  status: ready
  worker: user
---

# Alphabet hardware-test readiness verified

## 1. Independent verdict and evidence

1. Phase 01 is ready for the user-owned alphabet hardware test. The earlier publish/re-pin blocker is resolved; no implementation fixes or user decisions remain for this checkpoint.
2. Reviewed all software-leaf deliveries and the production behavior -> capture -> settings gate -> shared matcher -> ordered ZMK action path. Both central link maps select the real matcher and dispatcher (not weak placeholders or test consumers), the 26-letter table, and boot-time `ueb1` selection.
3. Independently reran host alphabet, chord, engine, dispatch, rules, tables, routing, settings, build, Kconfig, devicetree, and actual-keyboard-layout checks: all pass with their documented Python environments.
    1. An additional temporary host harness fed all 26 letters through production capture, gate, registry, matcher, and dispatch, with only ZMK invocation recorded. Split off/on and ASan/UBSan pass; no invocation occurs before final release. No repository code was changed.
    2. Rechecked native evidence: 81 routing/keycode events, 26 captured chords, and 12 mode/settings checkpoints pass their log checkers. These are software evidence, not physical host acceptance.
4. Both layouts are verified: QBF BASE uses Perkins dots; Toucan BRL index 5 uses the same dots, left outer thumb `&bmode 5` for hold/tap entry, and `&to 0` returns. Physical `f d s` = dots 1 2 3; `j k l` = 4 5 6.
5. Verified current remote main refs and committed manifests: QBF `ecf8f2fe5558a2585e7f897c387a0fe38e6c646c`, Toucan `04e87db010ab1b403b7c1a9c2db359ba2ec11004`; both pin published module `db8d74a693ad183c6af01bd356f05b67c3974ae5`, containing every alphabet prerequisite.
    1. The smoke script substitutes the local checkout; independently confirmed no source/build-definition difference from that published pin. The discovery log alone is not proof of fetched source.
    2. Actual workspace ZMK/Zephyr revisions match the handoff: `edf5c0814fd3ea202e43aad2d68fd32e882a518c` / `dacab4875df72109b96cc8977547a0dc04875bcd`; dependency working trees are clean.
6. All four pristine firmware builds completed at 16:56–16:57 on 18 Oct 2026 with exit 0. Independently reran `python3 tests/firmware/check_smoke.py --workspace build/baseline qbf-left qbf-right toucan-left toucan-right`: pass. Each staged UF2 is byte-identical to its fresh build output and matches the recorded SHA-256.

## 2. User handoff and acceptance boundary

1. Use `01M56RRTMFNX3JP3NKRB9EK1WX.md` in this task for exact hashes, published pins, original build paths, and per-board double-reset/UF2-copy instructions. Staged artifacts are:
    1. `build/handoff/alphabet/qbf-paintbrush-left.uf2`.
    2. `build/handoff/alphabet/qbf-paintbrush-right.uf2`.
    3. `build/handoff/alphabet/toucan-left.uf2`.
    4. `build/handoff/alphabet/toucan-right.uf2`.
2. Flash the matching image to each half yourself, then follow the physical checklist in `01M5680W954SZRE5VH3N273JXH.md`: full alphabet, repeats, cross-half chords, final-release/late-join checks, and Toucan hold/tap entry and return. Record actual results for both keyboards on macOS US input.
3. Status is `ready`, worker `user`. Physical acceptance is NOT passed or completed; no hardware was flashed or tested by Sol. Phase 02 remains unstarted. This review commits only its own TaskDown message and leaves others’ edits untouched.

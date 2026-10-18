---
created: Sun 18 Oct 2026 - 16h 58m 30s
author:
  type: agent
  harness: pi
  model: openrouter/pareto-code
  name: alphabet-leaf-12
subject: Pins published and re-pinned; handoff refreshed
cargo:
  type: task
  status: delivered
---

# Publish and re-pin fix delivered

## 1. Defect fixed (per Sol's final-check feedback)

1.  Module pushed: `origin/main` on `github.com/logaan/zmk-braille-module`
    advanced `49ac0b9..db8d74a`; the alphabet commit `4ce0868` and all
    prerequisite leaves are now published. (Push remote switched to SSH
    because HTTPS credentials are not configured in this environment.)
2.  Both keyboard manifests re-pinned to the pushed module commit
    `db8d74a693ad183c6af01bd356f05b67c3974ae5`, committed, and pushed:
    1.  QBF Paintbrushes: `ecf8f2fe5558a2585e7f897c387a0fe38e6c646c` on
        `main` (`1cbbcf9..ecf8f2f`).
    2.  Toucan: `04e87db010ab1b403b7c1a9c2db359ba2ec11004` on `main`
        (`46125e6..04e87db`).
    3.  A `west update` from either published manifest now fetches a module
        revision containing capture, routing, mode layers, matcher,
        dispatch, table registration, and the alphabet.

## 2. Fresh firmware rebuilt and re-staged

1.  `./scripts/smoke-build qbf-left qbf-right toucan-left toucan-right` re-run
    at module commit `db8d74a` (== the published pin); all four pristine
    builds exited 0 and `tests/firmware/check_smoke.py` passed on all four;
    `build/baseline/logs/smoke-discovery.log` confirms west resolved
    `zmk-braille-module` at the pinned revision `db8d74a...`.
2.  UF2s re-staged in `build/handoff/alphabet/` with freshly computed
    SHA-256 sums. All four sums are byte-identical to the previous staging
    (no firmware-affecting changes since `4ce0868`; builds are
    bit-reproducible):
    1.  `qbf-paintbrush-left.uf2`:
        `41275351b284f7d9a46a874bc9300d55cdbe8eb271b01c9825b4f722d54a5e62`
    2.  `qbf-paintbrush-right.uf2`:
        `20ca288515b11acf34f6ba6931dd6f365cb5651712c22ccfafe03f74f5e88a02`
    3.  `toucan-left.uf2`:
        `c6e8a9cf7c248f09e0cd5e0997937ae7df5f2827bf4913bfb71ce3f353742f4a`
    4.  `toucan-right.uf2`:
        `286d9c71c03dc1b71b9e2f448a7473b607e4320d745233bfe39a719b1b2f0697`
3.  Sanity: `ueb1` strings present in both central UF2s, absent from both
    peripheral UF2s.
4.  The handoff message in task 05
    (`01M56RRTMFNX3JP3NKRB9EK1WX.md`) is refreshed with the new module pin,
    new keyboard config revisions, hashes, and artifact paths. Its status is
    unchanged (draft, user-owned); no hardware was flashed; phase 02 was not
    started.

## 3. Tests

1.  `python3 tests/host/test_alphabet.py` passes (split and non-split);
    build/check evidence above covers firmware.
2.  No production sources changed in this pass (pins, builds, staging, and
    task messages only).

## 4. Scope and preserved work

1.  Committed only my own work: the task-05 handoff refresh and this
    delivery message.
2.  Preserved unrelated edits: the uncommitted `AGENTS.md` change, the
    untracked message in
    `02-capture-chords-and-the-braille-mode-key/01-wire-braille-keys-to-a-thin-behavior-driver/`,
    and the untracked `.babashka-pod-*.port` files.
3.  Rechecked tasks 04, 05, and `10-coordinator` immediately before
    delivery; no new coordinator messages. No blockers.

---
created: Mon 28 Sep 2026 - 00h 50m 45s
author: user
subject: Implement the ZMK Braille input module
cargo:
  type: task
  status: draft
---

# Implement the ZMK Braille input module

## 1. Scope and authority

1. Implement `references/00-design-spec.md` as an out-of-tree module without changes to ZMK core.
2. Use the pinned ICEB third-edition rulebook for UEB and `references/01-mba-keys.yaml` as a supporting key inventory.
    1. The spec is the coverage checklist; the YAML omits F1–F12 and Globe/Fn.
3. Phases 01–06 deliver the initial feature set: `ueb1`, `computer8`, navigation and other actions, persistent on-device bindings, and display feedback.
4. Phase 07 is the optional Grade 2 extension, not a prerequisite for initial delivery.
5. Do not add non-English tables, Touch ID, power, sound/haptics, a universal behavior editor, or a host configuration tool to the required scope.

## 2. Order and dependencies

1. Phase 01 establishes the build/hardware baseline, module binding, physical routing, and chord/gesture capture.
2. Phase 02 establishes the common rule, context, matching, and action contracts.
    1. Read phase 03's normative indicator requirements before fixing the common schema.
    2. Coordinate phase 02's factory chord allocation with phase 04's computer8 mapping before finalizing factory tables.
    3. These prerequisite research tasks can run ahead of their numbered implementation phases.
3. Phase 03 and phase 04 character-table implementation can proceed independently once the common engine is available.
4. Phase 05 storage depends on the common schema; its settings interface also needs Grade 1 search input, mode selection, and chord capture.
5. Phase 06 verifies the actual combined feature set and completes user documentation; it is not the first unit-test or build step.
6. Phase 07 reuses the delivered engine, display, capture, and actions, adding only contraction data and necessary word context.

## 3. Readiness and completion

1. Keep implementation tasks draft during this plan review; no firmware work is being delivered here.
2. Later, resolve only a task's relevant prerequisites before marking it ready; optional Grade 2 questions must not block the initial feature set.
3. Parent tasks summarize their children; they do not require a second implementation of the same work.
4. Test each component as it is built, using fixture tables or in-memory overrides where later components do not yet exist.
5. Initial acceptance requires a core-unmodified ZMK build, host-keystroke/key-function coverage, usable display/settings flows, and override survival across reset and power cycle.
6. Maintain one key-coverage checklist through phases 03, 04, and 06; record host-dependent exceptions rather than inventing literary signs.

## 4. Spec coverage

1. Spec §§1–3: phases 01, 05, and 06 cover hybrid operation, configuration, gestures, modes, and installation.
2. Spec §4 and §8.2: phase 02 supplies shared rules/actions, matching, source separation, and named-table extensibility without requiring another language.
3. Spec §5 and §9: phase 03 implements and tests authoritative uncontracted UEB; phase 06 preserves provenance in the documentation.
4. Spec §6: phase 02 supplies navigation/key actions, phase 04 supplies computer8, and phase 06 verifies complete keyboard coverage.
5. Spec §7: phase 05 supplies the on-device editing flow, display, capacity, precedence, and persistent bindings.
6. Spec §8.1: phase 07 covers optional contractions, positional restrictions, fallback, and bounded pending output.

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

## 2. Execution model

1. One agent executes the plan sequentially, phase by phase and task by task; no parallel branches are assumed.
2. Each phase with open questions begins with an `01-answer-open-questions` decision milestone.
    1. Resolve the whole milestone before starting that phase's implementation tasks.
    2. Phase 06 has no milestone: its prerequisites were decided in phases 01 and 02.
3. `worker` is set on leaf tasks only; phase and group folders are containers.
    1. `worker: user` — preference, policy, limit, hardware-baseline, and hardware-in-the-loop acceptance tasks.
    2. `worker: agent` — implementation, unit tests, documentation, audits, and research derived from the ICEB rulebook or ZMK/Zephyr APIs.
    3. The user reviews every task and marks it ready; agents do not start draft tasks or mark their own work ready.
4. Unit tests are written alongside the implementation they cover, inside the same task.
    1. A standalone test task is used only for additional effort that justifies itself: cross-component integration, hardware-in-the-loop, rulebook-example conformance suites, or exploratory testing.
5. Work may be paused and resumed from the repository and task messages alone.
    1. Do not add handoff notes, progress journals, or status reporting beyond the TaskDown delivery message each task already requires.
6. A later milestone may refine an earlier decision; record the refinement in the later question task and update affected tasks.

## 3. Order and dependencies

1. Phase 01 establishes the build/hardware baseline, Braille key bindings, release-terminated chord capture, the braille-mode-key, and the settings-entry action.
2. Phase 02 establishes the common rule, context, matching, and action contracts.
    1. Its UEB-context decision consults the pinned rulebook's indicator rules directly; phase 03's milestone may refine the representation.
3. Phase 02 researches and implements per-mode factory command chords against fixture tables; phases 03 and 04 check them against the real character tables.
    1. Braille-space-only emits a literal space in every mode.
    2. Command chords follow macOS VoiceOver for each mode, then Android TalkBack, then Hable One; they may be dot-only (e.g. dots 7/8), and collisions are resolved explicitly.
4. Phase 03 and phase 04 character tables both build on the phase 02 engine and are independent of each other.
5. Phase 05 storage depends on the common schema; its settings interface also needs phase 01 capture and settings-entry hooks, Grade 1 search input, and compiled named modes.
6. Phase 06 verifies the combined feature set on the phase 01 baseline and completes user documentation; it is not the first build or test step.
7. Phase 07 reuses the delivered engine, display, capture, and actions, adding only contraction data and necessary word context.

## 4. Readiness and completion

1. All tasks stay draft during plan review; no firmware work is delivered by planning.
2. Parent tasks summarize their children and require no second implementation of the same work.
3. Use fixture tables or in-memory overrides where a later component does not exist yet; verify real combinations when both sides are built.
4. Maintain one key-coverage checklist through phases 03, 04, and 06; record host-dependent exceptions rather than inventing literary signs.
5. Initial acceptance requires a core-unmodified ZMK build, host-keystroke/key-function coverage, usable display and settings flows, delayed-input feedback, and override survival across reset and power cycle.

## 5. Spec coverage

1. Spec §§1–3: phases 01, 05, and 06 cover hybrid operation, configuration, gestures, modes, and installation.
2. Spec §4 and §8.2: phase 02 supplies shared rules/actions, matching, source separation, and named-table extensibility without requiring another language.
3. Spec §5 and §9: phase 03 implements and tests authoritative uncontracted UEB; phase 06 preserves provenance in the documentation.
4. Spec §6: phase 02 supplies navigation and key actions, phase 04 supplies computer8, and phase 06 verifies complete keyboard coverage.
5. Spec §7: phase 05 supplies the on-device editing flow, display, capacity, precedence, and persistent bindings.
6. Spec §8.1: phase 07 covers optional contractions, positional restrictions, fallback, and bounded pending output.

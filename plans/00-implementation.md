# UEB Grade 1 input module implementation plan

## Objective and scope

Build a separate ZMK feature module, named `zmk-feature-braille`, in the requested `braille-module` repository. Collect the union of six dot presses during a stroke; finish that stroke when all dot keys are up. Decode a stream of strokes as UEB Grade 1 (uncontracted) text. Indicators can consume strokes without producing text, and some symbols or state transitions require more than two strokes. Do not model this as one combo or one output character per cell.

Output initially means ordinary text through keyboard HID, not Unicode Braille cells, a refreshable display protocol, or a contracted Braille translator. Uncontracted UEB still requires indicators and multi-cell symbols. Distinguish uncontracted transcription from the UEB technical term “grade 1 mode”; determine its necessary indicators and scope from the rules rather than treating the terms as interchangeable.

The first deliverable is a deliberately bounded input profile; do not claim full UEB Grade 1 support until the rule coverage matrix is complete. Mark literary formatting/typeform and technical symbols that cannot be represented as plain text explicitly; do not silently discard them. Grade 2 contractions and automatic translation into contractions are out of scope.

## Normative source and compatibility baseline

Use the vendored [ICEB Rules of Unified English Braille, Third Edition 2024](../references/iceb/rules-of-unified-english-braille-2024.pdf) as the normative baseline. [Reference provenance and license](../references/README.md) records its official source, checksum, unchanged redistribution terms, and the distinction between this rulebook and later technical-guidelines updates. Cite exact sections in mappings and tests. Recheck applicable official corrections before implementation begins.

Initial rule coverage index (printed page numbers):

| Requirement | Rulebook sections | Printed pages |
| --- | --- | --- |
| Uncontracted scope versus grade 1 mode | 2.5, 5.11 | 14–15, 63 |
| Alphabet and letter modifiers | 4.1–4.6 | 45–54 |
| Grade 1 indicators, scope and numeric interaction | 5.1–5.11 | 57–63 |
| Numeric symbols, termination and grade 1 state | 6.1–6.5 | 65–68 |
| Numeric spaces and passages | 6.6–6.10 | 69–73 |
| Punctuation and quotation ambiguity | 7.1–7.7 | 75–88 |
| Capital symbol, word, passage and termination | 8.1–8.9 | 89–99 |
| General symbols and space | 3.1–3.28 | 21–43 |
| Typeform coverage decision | 9 | 101 onward |

Two implementation-critical distinctions are already verified: rule 2.5.2 distinguishes uncontracted braille from grade 1 mode; rules 6.3.1 and 6.5.1 give different termination conditions for numeric mode and numeric-induced grade 1 mode. Maintain separate states. Section 5.11 governs which grade 1 indicators are needed in entirely uncontracted text.

ZMK architecture was inspected locally at commit `9ebbeff0a8b69a42f14aec022cdf16c7a107b9e0` in the neighbouring checkout. Relevant upstream documentation: [modules](https://zmk.dev/docs/features/modules), [module creation](https://zmk.dev/docs/development/module-creation), and [new behaviors](https://zmk.dev/docs/development/new-behavior). Relevant local source paths: `app/include/zmk/event_manager.h`, `app/include/zmk/events/position_state_changed.h`, `app/src/combo.c`, `app/src/behavior_queue.c`, and `app/src/keymap.c`. Pin this revision for the first integration tests; do not assume behavior of newer upstream code.

## Architecture

Separate four components so the language logic is independently testable:

1. Position-event arbitration and mode control: decide which physical events belong to Braille, buffer tentative entry, and pass ordinary typing to ZMK exactly once.
2. Stroke collector: maintain `held_mask` and `stroke_mask`; OR dots into the latter on every press. Emit a cell only on a nonempty stroke's transition to zero held dots, then reset the stroke mask. Releasing one dot while another remains held does not finish a stroke. A dot re-pressed during the same stroke remains part of that cell. There is no maximum duration or inter-dot deadline inside an admitted stroke.
3. Pure UEB stream decoder: accept cells, spaces, explicit editing commands, and reset events; maintain language state and return zero or more semantic output tokens. Use bounded state and rule-indexed tables, independent of Zephyr and HID.
4. Output adapter: turn semantic text tokens into ordered press/release actions. Begin with an explicitly documented US host layout. Keep Unicode/non-US output behind an adapter contract; firmware keycodes alone do not universally encode arbitrary Unicode text.

Track press ownership through release. Mode changes must not leak the release of a consumed press, swallow the release of a passed-through press, or leave host keys held. Serialise input, timers, state transitions, and output queue updates in one execution context or protect them explicitly.

## ZMK integration and entry-order feasibility spike

Use an external module with `zephyr/module.yml` pointing to root `CMakeLists.txt`, `Kconfig`, and `dts_root`. Suggested files: `src/input.c`, `src/stroke.c`, `src/ueb.c`, `src/output.c`, internal headers, `dts/bindings/`, optional behavior definitions under `dts/behaviors/`, `tests/`, and `examples/`. Select an explicit module code license before distributing implementation; third-party reference terms remain separate.

Expose configured physical positions for dots 1–6, the mode key, space, cancel, and editing controls; validate uniqueness and ranges. Use logical keymap positions after keyboard transforms, never raw matrix coordinates. Bindings for explicit enable/disable/reset may supplement the physical mode key. The mode flag is module-owned; an optional layer may provide other Braille-mode keys but must not determine admission of the first stroke.

The input listener must see events early enough to arbitrate before combos and normal keymap/hold-tap handling. Verify the actual event-manager subscription order and capture/release APIs on the pinned checkout. Link order is not an acceptable undocumented guarantee. Prototype module-only ordering and safe replay first. If supported APIs cannot provide ordering, document the smallest upstream hook required; do not claim the simultaneous-entry requirement is satisfied by a late behavior callback. This is the primary architectural go/no-go milestone.

On split keyboards, collect and decode centrally after position events arrive from both halves. Verify that peripheral builds neither translate nor duplicate output, and measure delivery skew. Preserve event source, position, timestamp, and order during deferred replay.

## Mode key and ordinary typing

- Tap alone within a configurable tap term flips a persistent latch. Hold alone enters momentary mode after that term; a dot press while the mode key is down enters immediately and marks the mode key used, suppressing its tap action.
- Effective mode is the latch or momentary activation. A momentary hold must not clear an existing latch on release. A plain tap while latched turns it off.
- Support dots arriving shortly before the mode key with a configurable entry grace window. While off, buffer candidate dot-position events briefly. If the mode key arrives in time, admit the buffered stroke; otherwise replay the original events once in original order to normal ZMK processing. This unavoidable ordinary-typing latency must be documented and measured. The grace window applies only to entering mode, not to assembling an active stroke.
- If buffered dots are already all released before the mode press, replay as ordinary typing; do not reinterpret a completed ordinary keystroke as Braille. Specify boundaries exactly and test equal-timestamp/timer races.
- If the mode key releases before the last dot, retain ownership until that stroke completes. Decode it, then exit momentary mode and reset pending language state. A held mode key may cover many strokes and must not itself prevent stroke completion.
- Unrelated keys during tentative entry flush buffered ordinary events before themselves. In active mode, explicitly classify controls; do not allow space/reset midway through a stroke to reorder output. Initially defer space until the active stroke completes; cancel discards the stroke and drains its releases.
- Leaving mode resets incomplete prefixes and decoder modes after any admitted final stroke. Backspace first cancels a pending, un-emitted prefix; otherwise it sends a host backspace and resets context to a documented safe state. Do not pretend to reconstruct host text or restore deleted numeric/capitalisation context. Provide an explicit reset command.

## UEB decoding work

Create a rule coverage matrix with columns for official section, input cells, required context, transition, output, support status, and test IDs. Use these areas to expand the rule-indexed test matrix against the pinned edition:

- Six-dot numbering and uncontracted alphabet, including the expected `d + f + j` physical mapping to dots 1-2-4 and letter `f` when configured conventionally.
- Capital indicators: next-symbol, word, passage, and terminator forms; exact termination and interaction with punctuation and nonletters. Never implement “uppercase until space” as a substitute for the rules.
- Numeric indicator and a–j digit interpretation; exact continuation/termination rules, decimal/grouping symbols, intervening signs, and return to letters. Do not assume every punctuation mark or every nondigit has the same effect.
- Grade 1 indicators where needed to disambiguate symbols/letters, their scope, and interactions with numeric state. No contraction expansion.
- Multi-cell punctuation, symbols, prefixes, and terminators, including ambiguous prefixes and context-dependent punctuation requiring bounded lookahead. Specify word-boundary buffering and an explicit end-of-input/flush control where later context is needed; do not promise immediate character output after every non-indicator stroke. Use sections 7.5–7.6 to define quotation/question-mark interpretation and document any keyboard-specific disambiguation convention. A state transition may produce no text; a later cell may complete a symbol or force recovery.
- Typeform, accents, non-ASCII punctuation, and technical notation: classify required coverage and output capability explicitly. Track unsupported entries rather than silently substituting incorrect ASCII.

Represent capitalisation scope, numeric context, grade 1 scope, and pending symbol prefixes separately, with explicit transition functions. Use named dot masks and provenance comments. Choose an observable invalid/unsupported sequence policy (for example, consume the invalid pending sequence, reset affected state, and expose an error status); never emit a guessed character. Define whether the current cell is retried after a failed prefix and test it. Avoid retaining unbounded text history.

## Output and failure handling

Emit a bounded sequence of balanced HID/behavior press-release pairs per decoded token. The inspected ZMK behavior queue enqueues individual actions and can fail; do not assume a pair is atomic. Use a bounded module-owned transaction queue or a proven equivalent so exhaustion cannot enqueue a press without its release. Test repeated characters and capital output with physical modifiers already down; preserve modifiers the module does not own. Define whether physical Shift/Ctrl/Alt are allowed to affect Braille text or are disallowed in the initial profile.

On disconnect, suspend, reset, or lost split connection, cancel incomplete strokes/prefixes, release any module-owned output, and clear transient state. Decide explicitly whether the user latch survives each lifecycle event; default to off on reboot and connection-loss recovery. Use a recovery/reset path for missing releases, not a timeout that emits a partial character. Bound input buffers and report overflow without synthesising text or leaving stuck keys.

## Delivery milestones and acceptance gates

1. Reference acquisition and legal vendoring are complete. Expand the rule index above into the full coverage matrix. Confirm plain-text output scope before labelling the implementation complete.
2. Prove external-module discovery/build and event ordering/replay on the pinned ZMK revision. Show ordinary QWERTY passes through and mode-plus-first-stroke works for every press order within the admission window. Resolve the upstream-hook question before building the full decoder.
3. Implement and unit-test the pure collector and decoder. Start with letters, space, capital-symbol and numeric sequences; then complete all required scopes, multi-cell symbols, and recovery rules in the coverage matrix.
4. Integrate mode arbitration, bounded output, lifecycle cleanup, and split handling. Add a small example keymap and installation instructions using a pinned West project revision; use a local extra module path during development. No repository URL is assumed until a remote is created.
5. Run native decoder tests plus ZMK event/HID integration tests. Validate on a full-size keyboard and a split device; confirm the selected six keys and mode key support the required hardware rollover. Measure latency and resource use before publishing.

Required test families:

- All 63 nonempty dot masks; press/release permutations, staggered overlap, long pauses while a dot remains down, repeated dots, and consecutive identical cells. No output before all dots are up; exactly one cell event afterward.
- Indicator followed by letter/digit emits the expected single character; indicators alone emit nothing. Cover capital word/passage termination, number-to-letter ambiguity, punctuation in numeric contexts, multi-cell symbols, spaces, invalid input, cancellation, and mode exit using rule-cited vectors.
- Mode first/dots first, grace-window boundaries, tap/hold boundaries, latch plus hold, release mode before dots, multiple strokes per hold, and indicator-plus-character under one hold.
- QWERTY press/release fidelity, combo and hold-tap coexistence, layer changes, transparent bindings, physical modifiers, and unrelated-key ordering during buffering.
- Split delivery skew and source ownership, duplicate/missing releases, queue/buffer saturation, disconnect/suspend, and reset during pending output. No stuck key or unintended latch toggle.

Planning status: the separate Git repository, README, plan, and unchanged official reference are prepared. An opt-in external-module hello-world logging stub exists; no Braille input or translation has been implemented or firmware-tested.

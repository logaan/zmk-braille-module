// SPDX-License-Identifier: MIT

#ifndef BRAILLE_ENGINE_H
#define BRAILLE_ENGINE_H

#include <stdbool.h>
#include <stdint.h>

#include <braille_rules.h>
#include <zmk/behavior.h>

/*
 * The shared matcher and pending-input buffer. The selected character table,
 * that mode's factory command table (navigation and noncharacter functions),
 * and the user override layer are all matched through the same nine-bit input
 * path: `braille_chord_resolved` (defined by braille_engine.c) is the single
 * entry point, and every table is a `struct braille_rule_table` view.
 *
 * Matching rules (leaf requirements):
 *  - Prefer the longest valid sequence; for the same input, the applicable
 *    user override wins over a factory rule.
 *  - There is no timer: ambiguous input stays pending until the next chord
 *    extends it or rules it out. Ruled-out input commits the longest complete
 *    match, reprocesses the remaining cells exactly once, and drops an orphan
 *    incomplete prefix with a display notice; the chord that ruled it out is
 *    then interpreted from scratch.
 *  - A rule is a candidate only when its `when` predicate holds in the
 *    context captured at the start of its sequence; pending cells do not
 *    change the context. Committing a rule applies its class transition.
 *  - START needs word_start; END waits for whitespace/hyphen lookahead;
 *    MIDDLE needs neither a starting nor an ending boundary.
 *  - Space-only, dots-only, and space-plus-dot chords are treated
 *    identically; nothing branches on individual characters or commands.
 */

/* How a commit was triggered. The dispatch and display leaves consume these
 * to time word-boundary side effects; the matcher itself branches only on
 * WHITESPACE vs OTHER when a command chord rules out pending input. */
enum braille_commit_cause {
    /* The rule consumed the pending buffer and nothing can extend it. */
    BRAILLE_COMMIT_CLEAN = 0,
    /* The next chord ruled the pending input out. */
    BRAILLE_COMMIT_RULED_OUT,
    /* A whitespace chord (Braille space, or a Return or Tab command chord)
     * ruled the pending input out; the commit carries word-end evidence. */
    BRAILLE_COMMIT_WORD_END,
    /* A non-whitespace command chord ruled the pending input out; the pending
     * cells were discarded without host output. */
    BRAILLE_COMMIT_DISCARDED,
};

/* The factory command chords that act as whitespace when they rule out
 * pending input: Braille space, Return, and Tab. Set by
 * `braille_engine_set_tables` from the command table. */
struct braille_whitespace_chords {
    uint16_t space;
    uint16_t enter;
    uint16_t tab;
    /* Optional Return aliases (e.g. VoiceOver dot8 and space+dot8). Zero is
     * unused. Every alias must share the canonical Return's whitespace path. */
    uint16_t enter_aliases[2];
};

/* Select the active mode's tables and the override layer. All views must
 * stay valid until the next call. Passing NULL for `character` or `commands`
 * matches against the override layer alone. Both factory tables are validated
 * together: conflicting factory mappings within the mode (including between
 * its character and command tables) are rejected and the previous selection
 * is kept. Returns 0, -EINVAL on a factory collision. */
int braille_engine_set_tables(const struct braille_rule_table *character,
                              const struct braille_rule_table *commands);

/* Point the matcher at the (initially empty) override layer. The caller owns
 * the storage; the matcher re-reads `count` on every chord, so overrides
 * edited on the keyboard take effect immediately. */
void braille_engine_set_overrides(const struct braille_rule_table *overrides);

/* Supply the mode's whitespace command chords for pending-input resolution.
 * Called by the table-selection path; also usable directly by tests and the
 * later real command tables. */
void braille_engine_set_whitespace(const struct braille_whitespace_chords *whitespace);

/* True when `mask` is a whitespace chord for the selected mode. */
bool braille_engine_is_whitespace(uint16_t mask);

/* Discard pending input (without host output or a display notice) and reset
 * the UEB context. Called on any layer change, non-Braille key press,
 * character-mode change, other command action, or settings entry. */
void braille_engine_reset(void);

/* The engine-owned UEB context; pending cells never mutate it. */
const struct braille_context *braille_engine_context(void);

/* Number of chords currently held pending (0..BRAILLE_RULE_MAX_CHORDS). */
uint8_t braille_engine_pending_count(void);

/* Inspection view, valid until the next engine call. Only pending_count
 * entries are live; observers must copy cells they need to retain. */
const uint16_t *braille_engine_pending_cells(void);

/* Each named mode owns its pending bound (eight in the initial modes).
 * A future larger-buffer mode can raise the storage/schema bound separately.
 * Returns -EINVAL outside 1..BRAILLE_RULE_MAX_CHORDS; success resets input. */
int braille_engine_set_pending_limit(uint8_t limit);

enum braille_pending_change {
    BRAILLE_PENDING_UPDATED,
    BRAILLE_PENDING_COMMITTED,
    BRAILLE_PENDING_DISCARDED,
};
/* Phase-05 display lifecycle hook. UPDATED carries the live buffer;
 * COMMITTED/DISCARDED carry the consumed cells. pending_count/cells already
 * reflect the new state. Called synchronously; copy, do not retain pointers.
 * Reset/layer/key/settings discards are silent (no dropped-prefix notice). */
void braille_engine_pending_changed(enum braille_pending_change change,
                                    const uint16_t *cells, uint8_t count);

/* Factory-table validation: two factory rules conflict when their chord
 * sequences are identical (neither is a strict prefix of the other) and their
 * `when` predicates are not disjoint (some context satisfies both).
 * Intentional shorter/longer prefixes and same-cells rules with disjoint
 * predicates are not collisions. The override layer is exempt: it wins by
 * precedence, never by collision. */
bool braille_engine_when_disjoint(const struct braille_rule_when *a,
                                  const struct braille_rule_when *b);
bool braille_engine_rules_collide(const struct braille_rule *a, const struct braille_rule *b);
int braille_engine_validate_factory(const struct braille_rule_table *character,
                                    const struct braille_rule_table *commands);

/*
 * Hooks supplied by sibling leaves. The defaults are weak no-ops so the
 * matcher is testable in isolation.
 *
 * `braille_engine_commit_rule` runs the committed rule's ordered actions
 * (leaf 03, ordered ZMK behavior dispatch). The default stub emits nothing
 * and returns 0. `cause` carries the word-end/discard evidence; `event` is
 * the chord-resolution event, forwarded unchanged.
 *
 * `braille_engine_notify_dropped_prefix` is the display notice for an orphan
 * incomplete prefix dropped without host output. The default stub does
 * nothing; the display leaf overrides it.
 */
int braille_engine_commit_rule(const struct braille_rule *rule, enum braille_commit_cause cause,
                               struct zmk_behavior_binding_event event);
void braille_engine_notify_dropped_prefix(const uint16_t *cells, uint8_t count,
                                          struct zmk_behavior_binding_event event);

#endif /* BRAILLE_ENGINE_H */

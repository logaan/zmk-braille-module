// SPDX-License-Identifier: MIT

#ifndef BRAILLE_RULES_H
#define BRAILLE_RULES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zmk/behavior.h>

/*
 * The single common rule/action schema shared by read-only factory character
 * and command tables, the (initially empty) user override layer, and later
 * persisted user rules. Records are fixed-size so factory tables can be
 * static const arrays, the override layer a mutable in-memory array, and
 * persistence a plain serialization of the same struct. There is no separate
 * character, command, or custom-action output type: every rule emits up to
 * BRAILLE_RULE_MAX_ACTIONS ordered ZMK behavior bindings, each a behavior
 * device with up to two parameters (root plan 4.4).
 *
 * Chord masks are the nine-bit values defined in braille_chord.h: dots 1..8
 * in bits 0..7, Braille space in bit 8; dot-only, space-only, and
 * space-plus-dot masks are not distinguished here.
 *
 * UEB context follows references/research/ueb-indicator-context.yaml: three
 * fields (numeric, caps, word_start), an optional `when` predicate over them
 * on every rule, one class naming the kind of sign, and a `sets` value only
 * on capitals indicator rules. There is no per-character dispatch.
 */

/* Root plan 4.4: a rule matches up to 8 chords and invokes up to 16 ordered
 * behavior actions. */
#define BRAILLE_RULE_MAX_CHORDS 8
#define BRAILLE_RULE_MAX_ACTIONS 16

/* Plan-level word-position constraint (root plan 4.4.3). In the UEB context
 * representation the word-start constraint is the word_start field; ueb1
 * needs no word-middle or word-end rules, but the values exist for tables
 * that do. */
enum braille_word_position {
    BRAILLE_WORD_POSITION_ANY = 0,
    BRAILLE_WORD_POSITION_START,
    BRAILLE_WORD_POSITION_MIDDLE,
    BRAILLE_WORD_POSITION_END,
};

/* Tri-state for one `when` field. Per field: numeric on/off, caps active
 * (word or passage)/inactive, word_start true/false. ANY means the field is
 * absent from the predicate and every context matches. */
enum braille_condition {
    BRAILLE_CONDITION_ANY = 0,
    BRAILLE_CONDITION_SET,
    BRAILLE_CONDITION_CLEAR,
};

/* Optional predicate over the UEB context at the rule's first chord. A rule
 * is a candidate only when every non-ANY part holds; pending cells do not
 * change the context it is tested against. */
struct braille_rule_when {
    enum braille_condition numeric;
    enum braille_condition caps;
    enum braille_condition word_start;
};

/* Capitals mode. On caps-indicator rules, `sets` names the mode the
 * indicator sets on commit. */
enum braille_caps_mode {
    BRAILLE_CAPS_NONE = 0,
    BRAILLE_CAPS_WORD,
    BRAILLE_CAPS_PASSAGE,
};

/* UEB indicator context, held by the common engine and reset together on the
 * plan's reset events; changed only when a rule commits. */
struct braille_context {
    bool numeric;
    enum braille_caps_mode caps;
    bool word_start;
};

/* Reset state: numeric off, no capitals, next sign begins a word. */
#define BRAILLE_CONTEXT_INIT                                                                         \
    ((struct braille_context){.numeric = false, .caps = BRAILLE_CAPS_NONE, .word_start = true})

/* One value per rule naming the kind of sign; the engine applies that
 * class's context transition after the rule's actions run, so no rule needs
 * per-character code. Neutral covers computer8, command, and user override
 * rules, which take no part in UEB context. */
enum braille_rule_class {
    BRAILLE_CLASS_LETTER = 0,
    BRAILLE_CLASS_CAPITAL_LETTER,
    BRAILLE_CLASS_DIGIT,
    BRAILLE_CLASS_NUMERIC_PUNCTUATION,
    BRAILLE_CLASS_SPACE,
    BRAILLE_CLASS_HYPHEN,
    BRAILLE_CLASS_OPENING,
    BRAILLE_CLASS_OTHER,
    BRAILLE_CLASS_CAPS_INDICATOR,
    BRAILLE_CLASS_GRADE1_INDICATOR,
    BRAILLE_CLASS_NEUTRAL,
};

struct braille_rule {
    /* Nine-bit chord masks, first chord first; entries past chord_count are
     * zero. */
    uint16_t chords[BRAILLE_RULE_MAX_CHORDS];
    /* Ordered ZMK behavior bindings; entries past action_count are zero. */
    struct zmk_behavior_binding actions[BRAILLE_RULE_MAX_ACTIONS];
    uint8_t chord_count;  /* 1..BRAILLE_RULE_MAX_CHORDS */
    uint8_t action_count; /* 0..BRAILLE_RULE_MAX_ACTIONS; indicators may emit none */
    enum braille_word_position word_position;
    struct braille_rule_when when;
    enum braille_rule_class class;
    /* The capitals mode set on commit; meaningful only when class is
     * BRAILLE_CLASS_CAPS_INDICATOR, otherwise BRAILLE_CAPS_NONE. */
    enum braille_caps_mode sets;
};

_Static_assert(BRAILLE_RULE_MAX_CHORDS == 8, "root plan 4.4: up to 8 chords per rule");
_Static_assert(BRAILLE_RULE_MAX_ACTIONS == 16, "root plan 4.4: up to 16 actions per rule");

/* A read-only view over one table's rules. Factory tables hand out a view
 * over their static const storage; the override layer hands out a view over
 * its mutable storage, so the matcher sees one shape. */
struct braille_rule_table {
    const struct braille_rule *rules;
    size_t count;
};

static inline bool braille_condition_holds(enum braille_condition condition, bool set) {
    return condition == BRAILLE_CONDITION_ANY || (condition == BRAILLE_CONDITION_SET) == set;
}

/* True when every non-ANY part of `when` holds in `ctx`. Caps SET means a
 * capitalised word or passage is in effect. */
static inline bool braille_rule_when_holds(const struct braille_rule_when *when,
                                           const struct braille_context *ctx) {
    return braille_condition_holds(when->numeric, ctx->numeric) &&
           braille_condition_holds(when->caps, ctx->caps != BRAILLE_CAPS_NONE) &&
           braille_condition_holds(when->word_start, ctx->word_start);
}

#endif /* BRAILLE_RULES_H */

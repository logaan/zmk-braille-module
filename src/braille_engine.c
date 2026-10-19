// SPDX-License-Identifier: MIT

#include <errno.h>
#include <string.h>

#include <zephyr/toolchain.h>
#include <drivers/behavior.h>
#include <braille_chord.h>
#include <braille_engine.h>
#include <braille_settings.h>

/* One matcher for characters, commands and overrides. Context is captured at
 * the first pending cell and changes only on commit. No indicator dispatch,
 * timer, or per-character output path lives here. */
enum match_layer { LAYER_NONE, LAYER_FACTORY, LAYER_OVERRIDE };
struct match_result {
    const struct braille_rule *rule;
    enum match_layer layer;
    uint8_t chords;
};

static const struct braille_rule_table *character_table;
static const struct braille_rule_table *command_table;
static const struct braille_rule_table *override_table;
static struct braille_whitespace_chords whitespace;
static struct braille_context context = BRAILLE_CONTEXT_INIT;
static struct braille_context pending_ctx = BRAILLE_CONTEXT_INIT;
static uint16_t pending[BRAILLE_RULE_MAX_CHORDS];
static uint8_t pending_len;
static uint8_t pending_limit = BRAILLE_RULE_MAX_CHORDS;
/* A layer/settings/mode reset during a dispatched action must not be undone
 * by its old rule's transition, nor replay an old buffered tail. */
static uint32_t reset_generation;

__weak int braille_engine_commit_rule(const struct braille_rule *rule,
                                      enum braille_commit_cause cause,
                                      struct zmk_behavior_binding_event event) {
    (void)rule;
    (void)cause;
    (void)event;
    return 0;
}

__weak void braille_engine_notify_dropped_prefix(const uint16_t *cells, uint8_t count,
                                                 struct zmk_behavior_binding_event event) {
    (void)cells;
    (void)count;
    (void)event;
}

__weak void braille_engine_pending_changed(enum braille_pending_change change,
                                           const uint16_t *cells, uint8_t count) {
    (void)change;
    (void)cells;
    (void)count;
}

const struct braille_context *braille_engine_context(void) { return &context; }
uint8_t braille_engine_pending_count(void) { return pending_len; }
const uint16_t *braille_engine_pending_cells(void) { return pending; }

void braille_engine_set_whitespace(const struct braille_whitespace_chords *ws) {
    whitespace = ws ? *ws : (struct braille_whitespace_chords){0};
}

bool braille_engine_is_whitespace(uint16_t mask) {
    return mask != 0 &&
           (mask == whitespace.space || mask == whitespace.enter || mask == whitespace.tab ||
            mask == whitespace.enter_aliases[0] || mask == whitespace.enter_aliases[1]);
}

void braille_engine_set_overrides(const struct braille_rule_table *overrides) {
    override_table = overrides;
}

static bool conditions_disjoint(enum braille_condition a, enum braille_condition b) {
    return (a == BRAILLE_CONDITION_SET && b == BRAILLE_CONDITION_CLEAR) ||
           (a == BRAILLE_CONDITION_CLEAR && b == BRAILLE_CONDITION_SET);
}

bool braille_engine_when_disjoint(const struct braille_rule_when *a,
                                  const struct braille_rule_when *b) {
    return conditions_disjoint(a->numeric, b->numeric) ||
           conditions_disjoint(a->caps, b->caps) ||
           conditions_disjoint(a->word_start, b->word_start);
}

static bool position_at_start(const struct braille_rule *rule, const struct braille_context *ctx) {
    return (rule->word_position != BRAILLE_WORD_POSITION_START || ctx->word_start) &&
           (rule->word_position != BRAILLE_WORD_POSITION_MIDDLE || !ctx->word_start);
}

static bool position_at_end(const struct braille_rule *rule, bool boundary) {
    return (rule->word_position != BRAILLE_WORD_POSITION_END || boundary) &&
           (rule->word_position != BRAILLE_WORD_POSITION_MIDDLE || !boundary);
}

static bool same_sequence(const struct braille_rule *a, const struct braille_rule *b) {
    return a->chord_count == b->chord_count &&
           memcmp(a->chords, b->chords, a->chord_count * sizeof(uint16_t)) == 0;
}

bool braille_engine_rules_collide(const struct braille_rule *a, const struct braille_rule *b) {
    if (!same_sequence(a, b) || braille_engine_when_disjoint(&a->when, &b->when)) {
        return false;
    }
    /* Include word-position constraints in collision validation. START and
     * END may overlap (a one-sign word); MIDDLE excludes both boundaries. */
    for (unsigned bits = 0; bits < 16; ++bits) {
        struct braille_context ctx = {
            .numeric = bits & 1, .caps = bits & 2 ? BRAILLE_CAPS_WORD : BRAILLE_CAPS_NONE,
            .word_start = bits & 4,
        };
        if (braille_rule_when_holds(&a->when, &ctx) &&
            braille_rule_when_holds(&b->when, &ctx) && position_at_start(a, &ctx) &&
            position_at_start(b, &ctx) && position_at_end(a, bits & 8) &&
            position_at_end(b, bits & 8)) {
            return true;
        }
    }
    return false;
}

static int validate_one(const struct braille_rule_table *t) {
    if (t && t->count && !t->rules) {
        return -EINVAL;
    }
    for (size_t i = 0; t && i < t->count; ++i) {
        const struct braille_rule *r = &t->rules[i];
        if (r->chord_count < 1 || r->chord_count > BRAILLE_RULE_MAX_CHORDS ||
            r->action_count > BRAILLE_RULE_MAX_ACTIONS ||
            r->word_position > BRAILLE_WORD_POSITION_END || r->class > BRAILLE_CLASS_NEUTRAL) {
            return -EINVAL;
        }
        for (uint8_t c = 0; c < r->chord_count; ++c) {
            if (!r->chords[c] || (r->chords[c] & ~BRAILLE_CHORD_MASK)) {
                return -EINVAL;
            }
        }
    }
    /* Validate sizes before comparing any sequences. */
    for (size_t i = 0; t && i < t->count; ++i) {
        for (size_t j = i + 1; j < t->count; ++j) {
            if (braille_engine_rules_collide(&t->rules[i], &t->rules[j])) {
                return -EINVAL;
            }
        }
    }
    return 0;
}

int braille_engine_validate_factory(const struct braille_rule_table *character,
                                    const struct braille_rule_table *commands) {
    if (validate_one(character) < 0 || validate_one(commands) < 0) {
        return -EINVAL;
    }
    for (size_t i = 0; character && i < character->count; ++i) {
        for (size_t j = 0; commands && j < commands->count; ++j) {
            if (braille_engine_rules_collide(&character->rules[i], &commands->rules[j])) {
                return -EINVAL;
            }
        }
    }
    return 0;
}

int braille_engine_set_tables(const struct braille_rule_table *character,
                              const struct braille_rule_table *commands) {
    int ret = braille_engine_validate_factory(character, commands);
    if (ret < 0) {
        return ret;
    }
    character_table = character;
    command_table = commands;
    pending_limit = BRAILLE_RULE_MAX_CHORDS;
    braille_engine_reset();
    return 0;
}

int braille_engine_set_pending_limit(uint8_t limit) {
    if (!limit || limit > BRAILLE_RULE_MAX_CHORDS) {
        return -EINVAL;
    }
    pending_limit = limit;
    braille_engine_reset();
    return 0;
}

void braille_engine_reset(void) {
    uint8_t old_len = pending_len;
    pending_len = 0;
    context = BRAILLE_CONTEXT_INIT;
    pending_ctx = BRAILLE_CONTEXT_INIT;
    ++reset_generation;
    if (old_len) {
        braille_engine_pending_changed(BRAILLE_PENDING_DISCARDED, pending, old_len);
    }
}

/* Strong implementation of the settings gate's weak reset hook. */
void braille_input_reset(void) { braille_engine_reset(); }

static bool sequence_is_command(const uint16_t *seq, uint8_t count) {
    for (size_t i = 0; command_table && i < command_table->count; ++i) {
        const struct braille_rule *r = &command_table->rules[i];
        if (r->chord_count == count && memcmp(r->chords, seq, count * sizeof(*seq)) == 0) {
            return true;
        }
    }
    return false;
}

static bool is_boundary(uint16_t cell, const struct braille_context *ctx) {
    if (braille_engine_is_whitespace(cell)) {
        return true;
    }
    /* Hyphens are class data, not a hard-coded cell or host-key test. */
    const struct braille_rule_table *sources[] = {override_table, character_table};
    for (size_t s = 0; s < ARRAY_SIZE(sources); ++s) {
        const struct braille_rule_table *t = sources[s];
        for (size_t i = 0; t && i < t->count; ++i) {
            const struct braille_rule *r = &t->rules[i];
            if (r->chord_count == 1 && r->chords[0] == cell &&
                r->class == BRAILLE_CLASS_HYPHEN && braille_rule_when_holds(&r->when, ctx)) {
                return true;
            }
        }
    }
    return false;
}

/* Longest complete valid prefix, whether the ENTIRE input is still a viable
 * prefix, and how much of an otherwise orphan prefix must be dropped. The
 * following cell is lookahead only: END needs a boundary, MIDDLE a nonboundary
 * character; neither consumes that following cell. */
static void match_sequence(const uint16_t *seq, uint8_t count, const struct braille_context *ctx,
                           struct match_result *best, bool *wait, uint8_t *orphan) {
    const struct braille_rule_table *sources[] = {override_table, character_table, command_table};
    memset(best, 0, sizeof(*best));
    *wait = false;
    *orphan = 0;
    for (size_t s = 0; s < ARRAY_SIZE(sources); ++s) {
        const struct braille_rule_table *t = sources[s];
        for (size_t i = 0; t && i < t->count; ++i) {
            const struct braille_rule *r = &t->rules[i];
            /* Overrides are mutable: never let an invalid row overrun storage. */
            if (!r->chord_count || r->chord_count > pending_limit ||
                r->action_count > BRAILLE_RULE_MAX_ACTIONS ||
                !braille_rule_when_holds(&r->when, ctx) || !position_at_start(r, ctx)) {
                continue;
            }
            uint8_t prefix = 0;
            while (prefix < r->chord_count && prefix < count && r->chords[prefix] == seq[prefix]) {
                ++prefix;
            }
            if (prefix > *orphan) {
                *orphan = prefix;
            }
            if (prefix < r->chord_count) {
                *wait |= prefix == count;
                continue;
            }
            if (r->word_position == BRAILLE_WORD_POSITION_END ||
                r->word_position == BRAILLE_WORD_POSITION_MIDDLE) {
                if (count == r->chord_count) {
                    *wait = true;
                    continue;
                }
                uint16_t next = seq[r->chord_count];
                if (!position_at_end(r, is_boundary(next, ctx)) ||
                    (r->word_position == BRAILLE_WORD_POSITION_MIDDLE &&
                     sequence_is_command(&next, 1))) {
                    continue;
                }
            }
            enum match_layer layer = s == 0 ? LAYER_OVERRIDE : LAYER_FACTORY;
            if (r->chord_count > best->chords ||
                (r->chord_count == best->chords && layer > best->layer)) {
                *best = (struct match_result){r, layer, r->chord_count};
            }
        }
    }
}

/* Class-based transitions from the researched context table. KEEP is encoded
 * as -1; capitals either stay, end word mode only, or take the rule's sets. */
static void transition(const struct braille_rule *rule) {
    enum { KEEP = -1, END_WORD = 0, SET_CAPS = 1 };
    static const struct { int8_t numeric, caps, word_start; } transitions[] = {
        [BRAILLE_CLASS_LETTER] = {0, KEEP, 0},
        [BRAILLE_CLASS_CAPITAL_LETTER] = {0, END_WORD, 0},
        [BRAILLE_CLASS_DIGIT] = {1, END_WORD, 0},
        [BRAILLE_CLASS_NUMERIC_PUNCTUATION] = {KEEP, END_WORD, 0},
        [BRAILLE_CLASS_SPACE] = {0, END_WORD, 1},
        [BRAILLE_CLASS_HYPHEN] = {0, END_WORD, 1},
        [BRAILLE_CLASS_OPENING] = {0, END_WORD, KEEP},
        [BRAILLE_CLASS_OTHER] = {0, END_WORD, 0},
        [BRAILLE_CLASS_CAPS_INDICATOR] = {0, SET_CAPS, KEEP},
        [BRAILLE_CLASS_GRADE1_INDICATOR] = {0, END_WORD, 0},
        [BRAILLE_CLASS_NEUTRAL] = {KEEP, KEEP, KEEP},
    };
    if ((unsigned)rule->class >= ARRAY_SIZE(transitions)) {
        return;
    }
    if (transitions[rule->class].numeric != KEEP) {
        context.numeric = transitions[rule->class].numeric;
    }
    if (transitions[rule->class].word_start != KEEP) {
        context.word_start = transitions[rule->class].word_start;
    }
    if (transitions[rule->class].caps == END_WORD && context.caps == BRAILLE_CAPS_WORD) {
        context.caps = BRAILLE_CAPS_NONE;
    } else if (transitions[rule->class].caps == SET_CAPS &&
               !(rule->sets == BRAILLE_CAPS_WORD && context.caps == BRAILLE_CAPS_PASSAGE)) {
        context.caps = rule->sets;
    }
}

static int commit(const struct braille_rule *rule, enum braille_commit_cause cause,
                  struct zmk_behavior_binding_event event) {
    bool ws = rule->chord_count == 1 && braille_engine_is_whitespace(rule->chords[0]);
    if (!ws && sequence_is_command(rule->chords, rule->chord_count)) {
        braille_engine_reset();
    }
    uint32_t generation = reset_generation;
    braille_engine_pending_changed(BRAILLE_PENDING_COMMITTED, rule->chords, rule->chord_count);
    int ret = braille_engine_commit_rule(rule, cause, event);
    if (ret < 0) {
        braille_engine_reset(); /* Never replay an action that may have partly executed. */
    } else if (generation == reset_generation) {
        if (ws) {
            const struct braille_rule space = {.class = BRAILLE_CLASS_SPACE};
            transition(&space); /* Return/Tab aliases share space's passage-preserving path. */
        } else {
            transition(rule);
        }
    }
    return ret;
}

int braille_chord_resolved(uint16_t mask, struct zmk_behavior_binding_event event) {
    mask &= BRAILLE_CHORD_MASK;
    if (!mask) {
        return 0;
    }
    uint16_t cells[BRAILLE_RULE_MAX_CHORDS + 1];
    uint8_t count = pending_len + 1;
    memcpy(cells, pending, pending_len * sizeof(*cells));
    cells[pending_len] = mask;
    struct braille_context ctx = pending_len ? pending_ctx : context;

    while (count) {
        struct match_result best;
        bool wait;
        uint8_t orphan;
        match_sequence(cells, count, &ctx, &best, &wait, &orphan);
        if (wait && count <= pending_limit) {
            memcpy(pending, cells, count * sizeof(*cells));
            pending_len = count;
            pending_ctx = ctx;
            braille_engine_pending_changed(BRAILLE_PENDING_UPDATED, pending, pending_len);
            return 0;
        }
        /* A command that does not complete/extend a sequence discards ALL
         * prior cells, even a complete or word-end candidate. Whitespace is
         * different: resolve the old cells at word end, then perform it. */
        if (count > 1 && (!best.rule || best.chords < count) &&
            sequence_is_command(&cells[count - 1], 1) &&
            !braille_engine_is_whitespace(cells[count - 1])) {
            uint8_t dropped = count - 1;
            pending_len = 0;
            braille_engine_reset();
            braille_engine_pending_changed(BRAILLE_PENDING_DISCARDED, cells, dropped);
            braille_engine_notify_dropped_prefix(cells, dropped, event);
            cells[0] = cells[count - 1];
            count = 1;
            ctx = context;
            continue;
        }
        pending_len = 0; /* Idle before dispatch; reentrant resets cannot resurrect cells. */
        uint8_t consumed;
        if (best.rule) {
            consumed = best.chords;
            enum braille_commit_cause cause = consumed == count ? BRAILLE_COMMIT_CLEAN :
                (is_boundary(cells[consumed], &ctx) ? BRAILLE_COMMIT_WORD_END :
                                                     BRAILLE_COMMIT_RULED_OUT);
            uint32_t generation = reset_generation;
            int ret = commit(best.rule, cause, event);
            if (ret < 0 || generation != reset_generation) {
                return ret;
            }
        } else {
            consumed = orphan ? orphan : 1;
            /* At most eight cells can be an incomplete prefix; a ninth is
             * lookahead, never stored. Every iteration consumes something. */
            braille_engine_pending_changed(BRAILLE_PENDING_DISCARDED, cells, consumed);
            braille_engine_notify_dropped_prefix(cells, consumed, event);
        }
        count -= consumed;
        memmove(cells, cells + consumed, count * sizeof(*cells));
        ctx = context; /* Re-evaluate every tail exactly once AFTER its predecessor commits. */
    }
    pending_ctx = context;
    return 0;
}

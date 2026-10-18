// SPDX-License-Identifier: MIT

#include <errno.h>
#include <string.h>

#include <zephyr/toolchain.h>
#include <drivers/behavior.h>
#include <braille_chord.h>
#include <braille_engine.h>

/*
 * Pending-input matcher. Chords arrive one at a time through
 * `braille_chord_resolved`. A rule is a candidate for a pending sequence
 * when its `when` predicate holds in the context captured at the start of
 * the sequence (pending cells never change that context, and this leaf never
 * mutates it: class-based transitions arrive with phase 02) and the pending
 * cells are a prefix of its chord list. Exact candidates (chord_count equal
 * to the pending length) may commit; strict-prefix candidates keep the
 * sequence extendable, so the buffer waits for the next chord instead of
 * committing on a timer.
 *
 * When the next chord extends the buffer it is appended and matching
 * repeats. When it rules the buffer out, the longest complete match commits,
 * the remaining cells are reprocessed exactly once, an orphan incomplete
 * prefix is dropped with a display notice, and the chord that ruled the
 * buffer out is interpreted from scratch. A whitespace chord (Braille space,
 * or the mode's Return/Tab command chords) commits pending input with
 * word-end evidence and then performs itself; any other command chord
 * discards pending input without host output and then performs itself.
 *
 * Space-only, dots-only, and space-plus-dot chords flow through the same
 * nine-bit comparisons; nothing branches on individual characters or
 * commands.
 */

/* Layer of a matched rule; higher layers win for the same input. */
enum match_layer {
    LAYER_NONE = 0,
    LAYER_FACTORY,  /* Selected character table or the mode's command table. */
    LAYER_OVERRIDE, /* User override layer. */
};

struct match_result {
    const struct braille_rule *rule;
    enum match_layer layer;
    uint8_t chords; /* Length of the matched (complete) sequence. */
};

static const struct braille_rule_table *character_table;
static const struct braille_rule_table *command_table;
static const struct braille_rule_table *override_table;
static struct braille_whitespace_chords whitespace;
static struct braille_context context = BRAILLE_CONTEXT_INIT;

static uint16_t pending[BRAILLE_RULE_MAX_CHORDS];
static uint8_t pending_len;
/* Context captured when the first pending cell arrived; `when` predicates
 * are tested against this, so pending cells do not change the context a rule
 * is candidate for. */
static struct braille_context pending_ctx = BRAILLE_CONTEXT_INIT;

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

const struct braille_context *braille_engine_context(void) { return &context; }

uint8_t braille_engine_pending_count(void) { return pending_len; }

void braille_engine_set_whitespace(const struct braille_whitespace_chords *ws) {
    whitespace = *ws;
}

bool braille_engine_is_whitespace(uint16_t mask) {
    return mask != 0 &&
           (mask == whitespace.space || mask == whitespace.enter || mask == whitespace.tab ||
            mask == whitespace.enter_aliases[0] || mask == whitespace.enter_aliases[1]);
}

void braille_engine_set_overrides(const struct braille_rule_table *overrides) {
    override_table = overrides;
}

/* True when one predicate requires a field set and the other requires it
 * clear; ANY never conflicts. */
static bool conditions_disjoint(enum braille_condition a, enum braille_condition b) {
    return (a == BRAILLE_CONDITION_SET && b == BRAILLE_CONDITION_CLEAR) ||
           (a == BRAILLE_CONDITION_CLEAR && b == BRAILLE_CONDITION_SET);
}

/* True when no context can satisfy both predicates. */
bool braille_engine_when_disjoint(const struct braille_rule_when *a,
                                  const struct braille_rule_when *b) {
    return conditions_disjoint(a->numeric, b->numeric) ||
           conditions_disjoint(a->caps, b->caps) ||
           conditions_disjoint(a->word_start, b->word_start);
}

/* Same sequence length and same cells. */
static bool same_sequence(const struct braille_rule *a, const struct braille_rule *b) {
    return a->chord_count == b->chord_count &&
           memcmp(a->chords, b->chords, a->chord_count * sizeof(uint16_t)) == 0;
}

bool braille_engine_rules_collide(const struct braille_rule *a, const struct braille_rule *b) {
    return same_sequence(a, b) && !braille_engine_when_disjoint(&a->when, &b->when);
}

static int validate_one(const struct braille_rule_table *t) {
    for (size_t i = 0; t && i < t->count; ++i) {
        const struct braille_rule *r = &t->rules[i];
        if (r->chord_count < 1 || r->chord_count > BRAILLE_RULE_MAX_CHORDS ||
            r->action_count > BRAILLE_RULE_MAX_ACTIONS) {
            return -EINVAL;
        }
        for (size_t j = i + 1; j < t->count; ++j) {
            if (braille_engine_rules_collide(r, &t->rules[j])) {
                return -EINVAL;
            }
        }
    }
    return 0;
}

/* Conflicts are rejected within each factory table and across the mode's
 * character and command tables; overrides never collide (they win by
 * precedence). */
int braille_engine_validate_factory(const struct braille_rule_table *character,
                                    const struct braille_rule_table *commands) {
    int ret = validate_one(character);
    if (ret < 0) {
        return ret;
    }
    ret = validate_one(commands);
    if (ret < 0) {
        return ret;
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
        return ret; /* Keep the previous selection. */
    }
    character_table = character;
    command_table = commands;
    braille_engine_reset();
    return 0;
}

void braille_engine_reset(void) {
    pending_len = 0;
    context = BRAILLE_CONTEXT_INIT;
    pending_ctx = BRAILLE_CONTEXT_INIT;
}

/* Longest complete match over the first `seq_len` cells of `seq`, with the
 * override layer preferred for the same input, and whether any candidate
 * extends past `seq_len`. Only rules whose `when` holds in `ctx` (the
 * context at the start of the sequence) are candidates. */
static void match_sequence(const uint16_t *seq, uint8_t seq_len, const struct braille_context *ctx,
                           struct match_result *best, bool *extendable) {
    static const struct {
        const struct braille_rule_table *const *table;
        enum match_layer layer;
    } sources[] = {
        {&override_table, LAYER_OVERRIDE},
        {&character_table, LAYER_FACTORY},
        {&command_table, LAYER_FACTORY},
    };

    memset(best, 0, sizeof(*best));
    *extendable = false;

    for (size_t s = 0; s < ARRAY_SIZE(sources); ++s) {
        const struct braille_rule_table *t = *sources[s].table;
        if (!t) {
            continue;
        }
        for (size_t r = 0; r < t->count; ++r) {
            const struct braille_rule *rule = &t->rules[r];
            if (rule->chord_count < 1 || !braille_rule_when_holds(&rule->when, ctx)) {
                continue;
            }
            uint8_t prefix = 0;
            while (prefix < rule->chord_count && prefix < seq_len &&
                   rule->chords[prefix] == seq[prefix]) {
                prefix++;
            }
            if (prefix < rule->chord_count) {
                /* Diverged within the available cells. */
                if (prefix == seq_len) {
                    *extendable = true; /* The next chord may complete it. */
                }
                continue;
            }
            /* Complete match; prefer the longest, then the highest layer. */
            if (rule->chord_count > best->chords ||
                (rule->chord_count == best->chords && sources[s].layer > best->layer)) {
                best->rule = rule;
                best->layer = sources[s].layer;
                best->chords = rule->chord_count;
            }
        }
    }
}

static bool sequence_is_command(const uint16_t *seq, uint8_t seq_len) {
    if (!command_table) {
        return false;
    }
    for (size_t r = 0; r < command_table->count; ++r) {
        const struct braille_rule *rule = &command_table->rules[r];
        if (rule->chord_count == seq_len &&
            memcmp(rule->chords, seq, seq_len * sizeof(uint16_t)) == 0) {
            return true;
        }
    }
    return false;
}

static uint8_t first_divergence(const struct braille_rule *rule, const uint16_t *seq,
                                uint8_t seq_len) {
    uint8_t i = 0;
    while (i < rule->chord_count && i < seq_len && rule->chords[i] == seq[i]) {
        i++;
    }
    return i;
}

/* True when no candidate rule could match any sequence starting with `seq`;
 * the cells are an orphan incomplete prefix. Complete matches of a strict
 * prefix are not checked: after a commit the matcher reprocesses the tail
 * from its own start, never a strict prefix. */
static bool prefix_is_orphan(const uint16_t *seq, uint8_t seq_len,
                             const struct braille_context *ctx) {
    static const struct braille_rule_table *const *sources[] = {
        &override_table, &character_table, &command_table,
    };
    for (size_t s = 0; s < ARRAY_SIZE(sources); ++s) {
        const struct braille_rule_table *t = *sources[s];
        if (!t) {
            continue;
        }
        for (size_t r = 0; r < t->count; ++r) {
            const struct braille_rule *rule = &t->rules[r];
            if (rule->chord_count > seq_len && braille_rule_when_holds(&rule->when, ctx) &&
                first_divergence(rule, seq, seq_len) == seq_len) {
                return false;
            }
        }
    }
    return true;
}

static int commit(const struct match_result *m, enum braille_commit_cause cause,
                  struct zmk_behavior_binding_event event) {
    return braille_engine_commit_rule(m->rule, cause, event);
}

static int drop_orphan(const uint16_t *cells, uint8_t count,
                       struct zmk_behavior_binding_event event) {
    braille_engine_notify_dropped_prefix(cells, count, event);
    return 0;
}

/*
 * Resolve a ruled-out pending buffer plus the chord `c` that ruled it out.
 * Whitespace chords (and only those) commit pending input as word-end
 * evidence; other command chords discard it without host output. The
 * remaining cells are reprocessed exactly once; an orphan incomplete prefix
 * is dropped with a display notice; `c` is then interpreted from scratch.
 */
static int resolve_ruled_out(uint16_t c, const struct braille_context *ctx,
                             struct zmk_behavior_binding_event event) {
    uint16_t cells[BRAILLE_RULE_MAX_CHORDS + 1];
    uint8_t n = pending_len;
    memcpy(cells, pending, n * sizeof(uint16_t));
    cells[n] = c;

    struct match_result m;
    bool ext;
    match_sequence(cells, n, ctx, &m, &ext);

    if (m.rule) {
        uint8_t tail = n - m.chords;
        bool c_is_command = sequence_is_command(&c, 1) && !braille_engine_is_whitespace(c);
        enum braille_commit_cause cause =
            c_is_command ? BRAILLE_COMMIT_DISCARDED
                         : (braille_engine_is_whitespace(c) ? BRAILLE_COMMIT_WORD_END
                                                            : BRAILLE_COMMIT_RULED_OUT);
        int ret;
        if (c_is_command) {
            /* Discard the pending input without host output. */
            braille_engine_notify_dropped_prefix(cells, n, event);
            pending_len = 0;
            ret = 0;
        } else {
            ret = commit(&m, cause, event);
            if (ret < 0) {
                return ret;
            }
            if (tail > 0) {
                /* Reprocess the remaining cells exactly once: they may begin
                 * a new pending sequence, but an orphan incomplete prefix is
                 * dropped with a display notice. */
                if (prefix_is_orphan(&cells[m.chords], tail, &context)) {
                    ret = drop_orphan(&cells[m.chords], tail, event);
                } else {
                    memcpy(pending, &cells[m.chords], tail * sizeof(uint16_t));
                    pending_len = tail;
                    pending_ctx = context;
                }
                if (ret < 0) {
                    return ret;
                }
            } else {
                pending_len = 0;
            }
        }
    } else if (n > 0) {
        /* No complete match in the pending cells: the whole buffer is an
         * orphan incomplete prefix. An empty buffer (a fresh chord that
         * matched nothing) is not an orphan and earns no notice. */
        int ret = drop_orphan(cells, n, event);
        if (ret < 0) {
            return ret;
        }
        pending_len = 0;
    }

    /* Interpret the chord that ruled the buffer out from scratch. */
    if (braille_engine_is_whitespace(c)) {
        struct match_result wm;
        bool wext;
        match_sequence(&c, 1, &context, &wm, &wext);
        if (wm.rule) {
            /* The whitespace chord performs itself after committing pending
             * input as at a word end. */
            return commit(&wm, BRAILLE_COMMIT_CLEAN, event);
        }
    } else if (sequence_is_command(&c, 1)) {
        struct match_result cm;
        bool cext;
        match_sequence(&c, 1, &context, &cm, &cext);
        if (cm.rule) {
            /* The command chord performs itself; pending input was already
             * discarded or committed without the command's cells. */
            pending_len = 0;
            return commit(&cm, BRAILLE_COMMIT_CLEAN, event);
        }
    }

    /* Ordinary chord, or one with no rule: start (or fail to start) a fresh
     * pending sequence from the current context. */
    struct match_result fm;
    bool fext;
    match_sequence(&c, 1, &context, &fm, &fext);
    if (fext) {
        pending[0] = c;
        pending_len = 1;
        pending_ctx = context;
        return 0;
    }
    if (fm.rule) {
        pending_len = 0;
        return commit(&fm, BRAILLE_COMMIT_CLEAN, event);
    }
    return drop_orphan(&c, 1, event);
}

int braille_chord_resolved(uint16_t mask, struct zmk_behavior_binding_event event) {
    mask &= BRAILLE_CHORD_MASK;
    if (mask == 0) {
        return 0; /* No Braille keys were pressed; nothing to match. */
    }
    if (pending_len == 0) {
        pending_ctx = context; /* The context at the start of the sequence. */
    }

    uint16_t cells[BRAILLE_RULE_MAX_CHORDS + 1];
    memcpy(cells, pending, pending_len * sizeof(uint16_t));
    cells[pending_len] = mask;

    struct match_result m;
    bool ext;
    match_sequence(cells, pending_len + 1, &pending_ctx, &m, &ext);

    if (ext) {
        if (pending_len >= BRAILLE_RULE_MAX_CHORDS) {
            return -EINVAL; /* Unreachable: no rule extends past 8 chords. */
        }
        memcpy(pending, cells, (pending_len + 1) * sizeof(uint16_t));
        pending_len++;
        return 0;
    }
    if (m.rule && m.chords == pending_len + 1) {
        /* Unambiguous complete match: commit immediately, no timer. */
        pending_len = 0;
        return commit(&m, BRAILLE_COMMIT_CLEAN, event);
    }
    /* The new chord rules the pending input out. */
    return resolve_ruled_out(mask, &pending_ctx, event);
}

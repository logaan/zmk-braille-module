// SPDX-License-Identifier: MIT
/*
 * Host tests for the pending-input matcher (src/braille_engine.c), compiled
 * against the stub ZMK headers and run natively by test_engine.py.
 *
 * Fixture tables are small factory character/command tables plus a mutable
 * in-memory override layer, because the real tables and persistence arrive
 * in later tasks. The commit and dropped-prefix hooks are overridden here to
 * record an event log; every expectation checks that log.
 */
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <braille_chord.h>
#include <braille_engine.h>
#include <braille_rules.h>
#include <zmk/behavior.h>

/* --- Chord masks (dots 1..8 in bits 0..7, Braille space in bit 8) -------- */

#define A 0x001   /* dot 1 */
#define C 0x003   /* dots 1+2 */
#define D 0x00f   /* dots 1+2+3+4 */
#define X 0x040   /* dot 7 */
#define SPC 0x100 /* Braille space */
#define SPD 0x101 /* Braille space + dot 1 */
#define RTN 0x008 /* dot 4: fixture Return command */
#define TAB 0x010 /* dot 5: fixture Tab command */
#define NAV 0x020 /* dot 6: fixture non-whitespace navigation command */
#define NUM 0x080 /* dot 8: fixture numeric-mode-only sign */

/* --- Event log ------------------------------------------------------------ */

enum event_kind { EV_COMMIT, EV_DROP };

struct log_entry {
    enum event_kind kind;
    const struct braille_rule *rule; /* EV_COMMIT only */
    enum braille_commit_cause cause; /* EV_COMMIT only */
    uint16_t cells[BRAILLE_RULE_MAX_CHORDS];
    uint8_t count; /* committed chord_count or dropped cell count */
};

static struct log_entry log_entries[32];
static size_t log_count;

static void log_reset(void) { log_count = 0; }

/* Every fixture rule carries a distinct first-action param1, which survives
 * the value copies into the table arrays; it stands in for rule identity. */
static bool log_is(size_t i, enum event_kind kind, const struct braille_rule *rule,
                   enum braille_commit_cause cause, uint8_t count, const uint16_t *cells) {
    const struct log_entry *e = &log_entries[i];
    return e->kind == kind && e->count == count &&
           (kind != EV_COMMIT ||
            (e->cause == cause && e->rule->action_count == rule->action_count &&
             e->rule->actions[0].param1 == rule->actions[0].param1)) &&
           memcmp(e->cells, cells, count * sizeof(uint16_t)) == 0;
}

/* Strong overrides of the matcher's weak hooks. */
int braille_engine_commit_rule(const struct braille_rule *rule, enum braille_commit_cause cause,
                               struct zmk_behavior_binding_event event) {
    (void)event;
    assert(log_count < sizeof(log_entries) / sizeof(log_entries[0]));
    struct log_entry *e = &log_entries[log_count++];
    e->kind = EV_COMMIT;
    e->rule = rule;
    e->cause = cause;
    e->count = rule->chord_count;
    memcpy(e->cells, rule->chords, rule->chord_count * sizeof(uint16_t));
    return 0;
}

void braille_engine_notify_dropped_prefix(const uint16_t *cells, uint8_t count,
                                          struct zmk_behavior_binding_event event) {
    (void)event;
    assert(log_count < sizeof(log_entries) / sizeof(log_entries[0]));
    struct log_entry *e = &log_entries[log_count++];
    e->kind = EV_DROP;
    e->rule = NULL;
    e->cause = 0;
    e->count = count;
    memcpy(e->cells, cells, count * sizeof(uint16_t));
}

/* --- Fixture rules and tables --------------------------------------------- */

#define WHEN_ANY                                                                                   \
    { BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY }

/* The letter [a] is a candidate only outside numeric mode, disjoint from
 * the numeric-mode digit [a] below (same cells, disjoint `when`). */
static const struct braille_rule r_a = {
    .chords = {A},
    .chord_count = 1,
    .actions = {{.behavior_dev = "&kp", .param1 = A}},
    .action_count = 1,
    .when = {BRAILLE_CONDITION_CLEAR, BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY},
    .class = BRAILLE_CLASS_LETTER,
};
static const struct braille_rule r_ac = {
    .chords = {A, C},
    .chord_count = 2,
    .actions = {{.behavior_dev = "&kp", .param1 = 0xac}},
    .action_count = 1,
    .when = WHEN_ANY,
    .class = BRAILLE_CLASS_LETTER,
};
static const struct braille_rule r_acd = {
    .chords = {A, C, D},
    .chord_count = 3,
    .actions = {{.behavior_dev = "&kp", .param1 = 0xacd}},
    .action_count = 1,
    .when = WHEN_ANY,
    .class = BRAILLE_CLASS_LETTER,
};
static const struct braille_rule r_x = {
    .chords = {X},
    .chord_count = 1,
    .actions = {{.behavior_dev = "&kp", .param1 = X}},
    .action_count = 1,
    .when = WHEN_ANY,
    .class = BRAILLE_CLASS_LETTER,
};
static const struct braille_rule r_spc = {
    .chords = {SPC},
    .chord_count = 1,
    .actions = {{.behavior_dev = "&kp", .param1 = 0x2c}},
    .action_count = 1,
    .when = WHEN_ANY,
    .class = BRAILLE_CLASS_SPACE,
};
/* Space-plus-dot: the same nine-bit path as any other chord. */
static const struct braille_rule r_spd = {
    .chords = {SPD},
    .chord_count = 1,
    .actions = {{.behavior_dev = "&kp", .param1 = SPD}},
    .action_count = 1,
    .when = WHEN_ANY,
    .class = BRAILLE_CLASS_OTHER,
};
/* Same cells as r_a but only in numeric mode: disjoint `when`, no collision. */
static const struct braille_rule r_a_numeric = {
    .chords = {A},
    .chord_count = 1,
    .actions = {{.behavior_dev = "&kp", .param1 = 0x1e}},
    .action_count = 1,
    .when = {BRAILLE_CONDITION_SET, BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY},
    .class = BRAILLE_CLASS_DIGIT,
};
/* Only a candidate at word start; the reset context has word_start true. */
static const struct braille_rule r_c_wordstart = {
    .chords = {C},
    .chord_count = 1,
    .actions = {{.behavior_dev = "&kp", .param1 = 0xc1}},
    .action_count = 1,
    .when = {BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_SET},
    .class = BRAILLE_CLASS_OPENING,
};
/* Only a candidate in numeric mode; never matches in the reset context. */
static const struct braille_rule r_num_only = {
    .chords = {NUM},
    .chord_count = 1,
    .actions = {{.behavior_dev = "&kp", .param1 = 0x0e}},
    .action_count = 1,
    .when = {BRAILLE_CONDITION_SET, BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY},
    .class = BRAILLE_CLASS_DIGIT,
};

static const struct braille_rule r_rtn = {
    .chords = {RTN},
    .chord_count = 1,
    .actions = {{.behavior_dev = "&kp", .param1 = 0x28}},
    .action_count = 1,
    .when = WHEN_ANY,
    .class = BRAILLE_CLASS_NEUTRAL,
};
static const struct braille_rule r_tab = {
    .chords = {TAB},
    .chord_count = 1,
    .actions = {{.behavior_dev = "&kp", .param1 = 0x2b}},
    .action_count = 1,
    .when = WHEN_ANY,
    .class = BRAILLE_CLASS_NEUTRAL,
};
static const struct braille_rule r_nav = {
    .chords = {NAV},
    .chord_count = 1,
    .actions = {{.behavior_dev = "&nav", .param1 = 1}},
    .action_count = 1,
    .when = WHEN_ANY,
    .class = BRAILLE_CLASS_NEUTRAL,
};

static const struct braille_rule char_rules[] = {r_a,   r_ac,          r_acd, r_x,        r_spc,
                                                 r_spd, r_a_numeric,   r_c_wordstart,     r_num_only};
static const struct braille_rule_table char_table = {
    .rules = char_rules,
    .count = sizeof(char_rules) / sizeof(char_rules[0]),
};
static const struct braille_rule cmd_rules[] = {r_rtn, r_tab, r_nav};
static const struct braille_rule_table cmd_table = {
    .rules = cmd_rules,
    .count = sizeof(cmd_rules) / sizeof(cmd_rules[0]),
};

/* Mutable in-memory override layer, edited by tests as the on-keyboard
 * editor will. */
static struct braille_rule override_storage[4];
static struct braille_rule_table override_table = {.rules = override_storage, .count = 0};

static const struct braille_rule r_ovr_a = {
    .chords = {A},
    .chord_count = 1,
    .actions = {{.behavior_dev = "&macro", .param1 = 0x0a}},
    .action_count = 1,
    .when = WHEN_ANY,
    .class = BRAILLE_CLASS_NEUTRAL,
};

static const struct zmk_behavior_binding_event ev = {.layer = 0, .position = 7, .timestamp = 1};

static void setup(void) {
    override_table.count = 0;
    braille_engine_set_overrides(&override_table);
    int ret = braille_engine_set_tables(&char_table, &cmd_table);
    assert(ret == 0);
    braille_engine_set_whitespace(
        &(struct braille_whitespace_chords){.space = SPC, .enter = RTN, .tab = TAB});
    braille_engine_reset();
    log_reset();
}

static void chord(uint16_t mask) {
    int ret = braille_chord_resolved(mask, ev);
    assert(ret == 0);
}

/* 1. Longest valid sequence wins over shorter valid ones; ambiguity waits
 *    for the next chord, never a timer. */
static void test_longest_valid_sequence(void) {
    setup();
    chord(A);
    assert(braille_engine_pending_count() == 1); /* [a] held: [a c] may extend */
    chord(C);
    assert(braille_engine_pending_count() == 2); /* [a c] held: [a c d] may extend */
    chord(D);
    assert(braille_engine_pending_count() == 0);
    uint16_t acd[] = {A, C, D};
    assert(log_count == 1);
    assert(log_is(0, EV_COMMIT, &r_acd, BRAILLE_COMMIT_CLEAN, 3, acd));

    /* [a c] commits when nothing extends it. */
    setup();
    chord(A);
    chord(C);
    chord(X);
    uint16_t ac[] = {A, C}, x[] = {X};
    assert(log_count == 2);
    assert(log_is(0, EV_COMMIT, &r_ac, BRAILLE_COMMIT_RULED_OUT, 2, ac));
    assert(log_is(1, EV_COMMIT, &r_x, BRAILLE_COMMIT_CLEAN, 1, x));
    printf("ok: longest valid sequence\n");
}

/* 2. Shorter fallback with the remaining input reprocessed exactly once:
 *    the orphan tail is dropped with one display notice and the breaking
 *    chord is interpreted from scratch. */
static void test_shorter_fallback_remaining_once(void) {
    setup();
    /* Pending [a] (extendable by [a c]); X rules it out. */
    chord(A);
    chord(X);
    uint16_t a[] = {A}, x[] = {X};
    assert(log_count == 2);
    assert(log_is(0, EV_COMMIT, &r_a, BRAILLE_COMMIT_RULED_OUT, 1, a));
    assert(log_is(1, EV_COMMIT, &r_x, BRAILLE_COMMIT_CLEAN, 1, x));
    assert(braille_engine_pending_count() == 0);

    /* Pending [a c] where only [a] is complete ([a c d] extends): the tail
     * cell C is reprocessed exactly once and, starting no rule, dropped with
     * one display notice. */
    static const struct braille_rule tail_rules[] = {r_a, r_acd, r_x};
    static const struct braille_rule_table tail_chars = {.rules = tail_rules, .count = 3};
    setup();
    assert(braille_engine_set_tables(&tail_chars, &cmd_table) == 0);
    chord(A);
    chord(C);
    chord(X);
    uint16_t c[] = {C};
    assert(log_count == 3);
    assert(log_is(0, EV_COMMIT, &r_a, BRAILLE_COMMIT_RULED_OUT, 1, a));
    assert(log_is(1, EV_DROP, NULL, 0, 1, c));
    assert(log_is(2, EV_COMMIT, &r_x, BRAILLE_COMMIT_CLEAN, 1, x));
    printf("ok: shorter fallback, remaining input once\n");
}

/* 3. An orphan incomplete prefix is dropped with a display notice and the
 *    chord that ruled it out is interpreted from scratch. */
static void test_orphan_prefix_dropped(void) {
    /* [a c] has no complete one-cell prefix in this table (r_a removed). */
    static const struct braille_rule no_a_rules[] = {r_ac, r_x};
    static const struct braille_rule_table no_a_chars = {.rules = no_a_rules, .count = 2};
    setup();
    assert(braille_engine_set_tables(&no_a_chars, &cmd_table) == 0);
    chord(A);
    assert(braille_engine_pending_count() == 1);
    chord(X); /* rules out the orphan [a] */
    uint16_t a[] = {A}, x[] = {X};
    assert(log_count == 2);
    assert(log_is(0, EV_DROP, NULL, 0, 1, a));
    assert(log_is(1, EV_COMMIT, &r_x, BRAILLE_COMMIT_CLEAN, 1, x));
    assert(braille_engine_pending_count() == 0);
    printf("ok: orphan prefix dropped\n");
}

/* 4. Whitespace chords (Braille space, Return, Tab) commit pending input
 *    with word-end evidence, then perform themselves. */
static void test_whitespace_commits_word_end(void) {
    setup();
    chord(A);
    chord(SPC);
    uint16_t a[] = {A}, spc[] = {SPC};
    assert(log_count == 2);
    assert(log_is(0, EV_COMMIT, &r_a, BRAILLE_COMMIT_WORD_END, 1, a));
    assert(log_is(1, EV_COMMIT, &r_spc, BRAILLE_COMMIT_CLEAN, 1, spc));

    setup();
    chord(A);
    chord(C);
    chord(RTN);
    uint16_t ac[] = {A, C}, rtn[] = {RTN};
    assert(log_count == 2);
    assert(log_is(0, EV_COMMIT, &r_ac, BRAILLE_COMMIT_WORD_END, 2, ac));
    assert(log_is(1, EV_COMMIT, &r_rtn, BRAILLE_COMMIT_CLEAN, 1, rtn));

    setup();
    chord(A);
    chord(TAB);
    uint16_t tab[] = {TAB};
    assert(log_count == 2);
    assert(log_is(0, EV_COMMIT, &r_a, BRAILLE_COMMIT_WORD_END, 1, a));
    assert(log_is(1, EV_COMMIT, &r_tab, BRAILLE_COMMIT_CLEAN, 1, tab));
    printf("ok: whitespace commits word-end evidence\n");
}

/* 5. A non-whitespace command chord discards pending input without host
 *    output, then performs itself. */
static void test_command_discards_pending(void) {
    setup();
    chord(A);
    chord(NAV);
    uint16_t a[] = {A}, nav[] = {NAV};
    assert(log_count == 2);
    assert(log_is(0, EV_DROP, NULL, 0, 1, a)); /* discarded: never committed */
    assert(log_is(1, EV_COMMIT, &r_nav, BRAILLE_COMMIT_CLEAN, 1, nav));
    assert(braille_engine_pending_count() == 0);
    printf("ok: command chord discards pending input\n");
}

/* 6. Candidate filtering by `when` against the sequence-start context;
 *    pending cells do not change that context. */
static void test_when_filtering(void) {
    setup();
    const struct braille_context *ctx = braille_engine_context();
    assert(!ctx->numeric && ctx->caps == BRAILLE_CAPS_NONE && ctx->word_start);

    /* numeric-only rule is not a candidate in the reset context. */
    chord(NUM);
    uint16_t num[] = {NUM};
    assert(log_count == 1);
    assert(log_is(0, EV_DROP, NULL, 0, 1, num));

    /* word-start-only rule is a candidate at word start. */
    setup();
    chord(C);
    uint16_t c[] = {C};
    assert(log_count == 1);
    assert(log_is(0, EV_COMMIT, &r_c_wordstart, BRAILLE_COMMIT_CLEAN, 1, c));

    /* Same cells, disjoint predicates: the non-numeric [a] letter wins over
     * the numeric-only [a] digit in the reset context. */
    setup();
    chord(A);
    chord(X);
    uint16_t a[] = {A}, x[] = {X};
    assert(log_count == 2);
    assert(log_is(0, EV_COMMIT, &r_a, BRAILLE_COMMIT_RULED_OUT, 1, a));
    assert(log_is(1, EV_COMMIT, &r_x, BRAILLE_COMMIT_CLEAN, 1, x));

    /* Pending cells do not change the context or which rules are candidates:
     * with [a c] pending, a whitespace chord still resolves against the
     * sequence-start context, and the context itself is untouched. */
    setup();
    chord(A);
    chord(C);
    assert(braille_engine_pending_count() == 2);
    ctx = braille_engine_context();
    assert(!ctx->numeric && ctx->caps == BRAILLE_CAPS_NONE && ctx->word_start);
    chord(SPC);
    uint16_t ac[] = {A, C}, spc[] = {SPC};
    assert(log_count == 2);
    assert(log_is(0, EV_COMMIT, &r_ac, BRAILLE_COMMIT_WORD_END, 2, ac));
    assert(log_is(1, EV_COMMIT, &r_spc, BRAILLE_COMMIT_CLEAN, 1, spc));
    printf("ok: when filtering against start context\n");
}

/* 7. The applicable user override beats the factory rule for the same
 *    input. */
static void test_override_precedence(void) {
    setup();
    override_storage[0] = r_ovr_a;
    override_table.count = 1;
    chord(A);
    chord(X); /* [a] ruled out of [a c]; override [a] beats factory [a] */
    uint16_t a[] = {A}, x[] = {X};
    assert(log_count == 2);
    assert(log_is(0, EV_COMMIT, &override_storage[0], BRAILLE_COMMIT_RULED_OUT, 1, a));
    assert(log_is(1, EV_COMMIT, &r_x, BRAILLE_COMMIT_CLEAN, 1, x));
    printf("ok: override precedence for identical inputs\n");
}

/* 8. A shorter override does not defeat a longer valid factory sequence. */
static void test_shorter_override_loses_to_longer_factory(void) {
    setup();
    override_storage[0] = r_ovr_a; /* [a] */
    override_table.count = 1;
    chord(A);
    chord(C);
    chord(D); /* [a c d] completes and commits, not the override [a] */
    uint16_t acd[] = {A, C, D};
    assert(log_count == 1);
    assert(log_is(0, EV_COMMIT, &r_acd, BRAILLE_COMMIT_CLEAN, 3, acd));

    setup();
    override_storage[0] = r_ovr_a;
    override_table.count = 1;
    chord(A);
    chord(C);
    chord(SPC); /* [a c] ruled out of [a c d]: still the longer factory rule */
    uint16_t ac[] = {A, C}, spc[] = {SPC};
    assert(log_count == 2);
    assert(log_is(0, EV_COMMIT, &r_ac, BRAILLE_COMMIT_WORD_END, 2, ac));
    assert(log_is(1, EV_COMMIT, &r_spc, BRAILLE_COMMIT_CLEAN, 1, spc));
    printf("ok: shorter override loses to longer valid sequence\n");
}

/* 9. Conflicting factory mappings are rejected; prefixes and disjoint
 *    predicates are not collisions. */
static void test_factory_collision_rejection(void) {
    /* Identical sequences with overlapping predicates: a real collision. */
    static const struct braille_rule dup_rules[] = {r_a, r_a};
    static const struct braille_rule_table dup = {.rules = dup_rules, .count = 2};
    assert(braille_engine_validate_factory(&dup, NULL) == -EINVAL);

    /* The same cells with disjoint `when` predicates: not a collision. */
    static const struct braille_rule disjoint_rules[] = {r_a, r_a_numeric};
    static const struct braille_rule_table disjoint = {.rules = disjoint_rules, .count = 2};
    assert(braille_engine_validate_factory(&disjoint, NULL) == 0);

    /* Intentional shorter/longer prefixes: not a collision. */
    static const struct braille_rule prefix_rules[] = {r_a, r_ac, r_acd};
    static const struct braille_rule_table prefixes = {.rules = prefix_rules, .count = 3};
    assert(braille_engine_validate_factory(&prefixes, NULL) == 0);

    /* Collisions are caught across the character and command tables. */
    static const struct braille_rule r_a_cmd = {
        .chords = {A},
        .chord_count = 1,
        .actions = {{.behavior_dev = "&nav", .param1 = 9}},
        .action_count = 1,
        .when = WHEN_ANY,
        .class = BRAILLE_CLASS_NEUTRAL,
    };
    static const struct braille_rule cross_cmd_rules[] = {r_nav, r_a_cmd};
    static const struct braille_rule_table cross_cmds = {.rules = cross_cmd_rules, .count = 2};
    assert(braille_engine_validate_factory(&char_table, &cross_cmds) == -EINVAL);

    /* set_tables rejects the colliding mode and keeps the previous
     * selection. */
    setup();
    assert(braille_engine_set_tables(&char_table, &cross_cmds) == -EINVAL);
    chord(A);
    chord(X); /* still the previously selected fixture tables */
    uint16_t a[] = {A}, x[] = {X};
    assert(log_count == 2);
    assert(log_is(0, EV_COMMIT, &r_a, BRAILLE_COMMIT_RULED_OUT, 1, a));
    assert(log_is(1, EV_COMMIT, &r_x, BRAILLE_COMMIT_CLEAN, 1, x));
    printf("ok: factory collision rejection\n");
}

/* 10. Space-only, dots-only, and space-plus-dot chords all flow through the
 *     same path; reset discards pending input and restores the context. */
static void test_uniform_chords_and_reset(void) {
    setup();
    chord(SPD); /* space-plus-dot commits like any other chord */
    uint16_t spd[] = {SPD};
    assert(log_count == 1);
    assert(log_is(0, EV_COMMIT, &r_spd, BRAILLE_COMMIT_CLEAN, 1, spd));

    setup();
    chord(A);
    assert(braille_engine_pending_count() == 1);
    braille_engine_reset();
    assert(braille_engine_pending_count() == 0);
    const struct braille_context *ctx = braille_engine_context();
    assert(!ctx->numeric && ctx->caps == BRAILLE_CAPS_NONE && ctx->word_start);
    assert(log_count == 0); /* reset drops pending without output or notice */
    printf("ok: uniform chords and reset\n");
}

int main(void) {
    test_longest_valid_sequence();
    test_shorter_fallback_remaining_once();
    test_orphan_prefix_dropped();
    test_whitespace_commits_word_end();
    test_command_discards_pending();
    test_when_filtering();
    test_override_precedence();
    test_shorter_override_loses_to_longer_factory();
    test_factory_collision_rejection();
    test_uniform_chords_and_reset();
    printf("all engine matcher tests passed\n");
    return 0;
}

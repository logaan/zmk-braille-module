// SPDX-License-Identifier: MIT
/* Common context/pending lifecycle tests, independent of UEB character data. */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <braille_chord.h>
#include <braille_engine.h>
#include <braille_settings.h>

#define COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define A 1
#define B 3
#define C 9
#define D 25
#define X 53
#define SPACE 0x100
#define ENTER 0x111
#define TAB 0x11e
#define NAV 0x101
#define HYPHEN 0x24
static const struct zmk_behavior_binding_event ev = {.position = 4, .timestamp = 123};
static unsigned output[32], output_count, drops, updates, commits, discards;
static bool reset_on_commit, fail_on_commit;

int braille_engine_commit_rule(const struct braille_rule *r, enum braille_commit_cause cause,
                               struct zmk_behavior_binding_event event) {
    (void)cause;
    assert(event.timestamp == ev.timestamp);
    assert(!braille_engine_pending_count());
    assert(output_count < COUNT(output));
    output[output_count++] = r->actions[0].param1;
    if (reset_on_commit) {
        braille_engine_reset();
    }
    return fail_on_commit ? -EIO : 0;
}
void braille_engine_notify_dropped_prefix(const uint16_t *cells, uint8_t count,
                                          struct zmk_behavior_binding_event event) {
    (void)cells;
    (void)event;
    assert(count && count <= BRAILLE_RULE_MAX_CHORDS);
    ++drops;
}
void braille_engine_pending_changed(enum braille_pending_change change,
                                    const uint16_t *cells, uint8_t count) {
    assert(cells && count && count <= BRAILLE_RULE_MAX_CHORDS);
    switch (change) {
    case BRAILLE_PENDING_UPDATED:
        ++updates;
        assert(braille_engine_pending_count() == count);
        assert(memcmp(cells, braille_engine_pending_cells(), count * sizeof(*cells)) == 0);
        break;
    case BRAILLE_PENDING_COMMITTED: ++commits; break;
    case BRAILLE_PENDING_DISCARDED: ++discards; break;
    }
}

#define RULE(cell, tag, cls) { .chords = {cell}, .chord_count = 1, \
    .actions = {{.param1 = tag}}, .class = cls }
static const struct braille_rule command_rules[] = {
    RULE(SPACE, 10, BRAILLE_CLASS_SPACE), RULE(ENTER, 11, BRAILLE_CLASS_NEUTRAL),
    RULE(TAB, 12, BRAILLE_CLASS_NEUTRAL), RULE(NAV, 13, BRAILLE_CLASS_NEUTRAL),
    RULE(0x80, 11, BRAILLE_CLASS_NEUTRAL), RULE(0x180, 11, BRAILLE_CLASS_NEUTRAL),
};
static const struct braille_rule_table commands = {command_rules, COUNT(command_rules)};
static struct braille_rule rules[8];
static struct braille_rule_table characters = {rules, 0};

static struct braille_rule rule(uint16_t cell, unsigned tag, enum braille_rule_class cls) {
    return (struct braille_rule)RULE(cell, tag, cls);
}
static void context_is(struct braille_context expected) {
    const struct braille_context *actual = braille_engine_context();
    assert(actual->numeric == expected.numeric && actual->caps == expected.caps &&
           actual->word_start == expected.word_start);
}
static void seed(struct braille_context ctx) {
    *(struct braille_context *)braille_engine_context() = ctx; /* test-only */
}
static void clear_log(void) { output_count = drops = updates = commits = discards = 0; }
static void setup(size_t count) {
    characters.count = count;
    braille_engine_set_overrides(NULL);
    assert(braille_engine_set_tables(&characters, &commands) == 0);
    braille_engine_set_whitespace(&(struct braille_whitespace_chords){
        .space = SPACE, .enter = ENTER, .tab = TAB, .enter_aliases = {0x80, 0x180}});
    reset_on_commit = fail_on_commit = false;
    clear_log();
}
static void chord(uint16_t cell) { assert(braille_chord_resolved(cell, ev) == 0); }

static void test_every_class_transition(void) {
    for (int cls = BRAILLE_CLASS_LETTER; cls <= BRAILLE_CLASS_NEUTRAL; ++cls) {
        for (int sets = BRAILLE_CAPS_NONE; sets <= BRAILLE_CAPS_PASSAGE; ++sets) {
            rules[0] = rule(A, 1, cls);
            rules[0].sets = sets;
            setup(1);
            for (unsigned numeric = 0; numeric < 2; ++numeric) {
                for (int caps = BRAILLE_CAPS_NONE; caps <= BRAILLE_CAPS_PASSAGE; ++caps) {
                    for (unsigned start = 0; start < 2; ++start) {
                        struct braille_context before = {numeric, caps, start}, expected = before;
                        seed(before);
                        clear_log();
                        /* Independent expectations from the research's class rows. */
                        if (cls != BRAILLE_CLASS_NUMERIC_PUNCTUATION && cls != BRAILLE_CLASS_NEUTRAL)
                            expected.numeric = cls == BRAILLE_CLASS_DIGIT;
                        if (cls == BRAILLE_CLASS_SPACE || cls == BRAILLE_CLASS_HYPHEN)
                            expected.word_start = true;
                        else if (cls != BRAILLE_CLASS_OPENING && cls != BRAILLE_CLASS_CAPS_INDICATOR &&
                                 cls != BRAILLE_CLASS_NEUTRAL)
                            expected.word_start = false;
                        if (cls == BRAILLE_CLASS_CAPS_INDICATOR) {
                            if (!(sets == BRAILLE_CAPS_WORD && caps == BRAILLE_CAPS_PASSAGE))
                                expected.caps = sets;
                        } else if (cls != BRAILLE_CLASS_LETTER && cls != BRAILLE_CLASS_NEUTRAL &&
                                   caps == BRAILLE_CAPS_WORD) {
                            expected.caps = BRAILLE_CAPS_NONE;
                        }
                        chord(A);
                        context_is(expected);
                        assert(commits == 1 && !updates && !drops && !discards);
                    }
                }
            }
        }
    }
    puts("ok: every class/sets transition in all twelve contexts");
}

static void test_positions_and_boundaries(void) {
    static const uint16_t boundaries[] = {SPACE, ENTER, TAB, 0x80, 0x180, HYPHEN};
    rules[0] = rule(A, 1, BRAILLE_CLASS_LETTER);
    rules[1] = rule(A, 2, BRAILLE_CLASS_LETTER);
    rules[1].chords[1] = B;
    rules[1].chord_count = 2;
    rules[1].word_position = BRAILLE_WORD_POSITION_END;
    rules[2] = rule(B, 3, BRAILLE_CLASS_LETTER);
    rules[3] = rule(X, 4, BRAILLE_CLASS_LETTER);
    rules[4] = rule(HYPHEN, 5, BRAILLE_CLASS_HYPHEN);
    for (size_t i = 0; i < COUNT(boundaries); ++i) {
        setup(5);
        chord(A);
        chord(B);
        assert(!output_count && braille_engine_pending_count() == 2);
        assert(braille_engine_pending_cells()[0] == A && braille_engine_pending_cells()[1] == B);
        context_is(BRAILLE_CONTEXT_INIT);
        chord(boundaries[i]);
        assert(output_count == 2 && output[0] == 2 && !drops && !braille_engine_pending_count());
    }
    setup(5);
    chord(A); chord(B); chord(X);
    assert(output_count == 3 && output[0] == 1 && output[1] == 3 && output[2] == 4);
    assert(!drops && !braille_engine_pending_count());
    setup(5);
    chord(A); chord(B); chord(NAV);
    assert(output_count == 1 && output[0] == 13 && drops == 1 && discards == 1);
    context_is(BRAILLE_CONTEXT_INIT);

    /* Same input, disjoint positions are legal, but ANY/START and START/END collide. */
    rules[0] = rule(A, 1, BRAILLE_CLASS_LETTER);
    rules[0].word_position = BRAILLE_WORD_POSITION_START;
    rules[1] = rule(A, 2, BRAILLE_CLASS_LETTER);
    rules[1].word_position = BRAILLE_WORD_POSITION_MIDDLE;
    rules[2] = rule(A, 3, BRAILLE_CLASS_LETTER);
    rules[2].word_position = BRAILLE_WORD_POSITION_END;
    rules[3] = rule(X, 4, BRAILLE_CLASS_LETTER);
    assert(!braille_engine_rules_collide(&rules[0], &rules[1]));
    assert(!braille_engine_rules_collide(&rules[1], &rules[2]));
    assert(braille_engine_rules_collide(&rules[0], &rules[2]));
    setup(2);
    chord(A);
    assert(output_count == 1 && output[0] == 1);
    chord(A);
    assert(output_count == 1 && braille_engine_pending_count() == 1);
    /* A later nonboundary character supplies the MIDDLE lookahead. */
    chord(A);
    assert(output_count == 2 && output[1] == 2 && braille_engine_pending_count() == 1);
    chord(SPACE); /* No END candidate here: discard the invalid MIDDLE only. */
    assert(output_count == 3 && output[2] == 10 && drops == 1);
    puts("ok: all word positions, real whitespace aliases/hyphen, fallback and command discard");
}

static void test_full_tail_reprocessing_and_context(void) {
    rules[0] = rule(A, 1, BRAILLE_CLASS_DIGIT);
    rules[1] = rule(B, 2, BRAILLE_CLASS_LETTER);
    rules[1].when.numeric = BRAILLE_CONDITION_SET;
    rules[2] = rule(C, 3, BRAILLE_CLASS_LETTER);
    rules[3] = rule(X, 4, BRAILLE_CLASS_LETTER);
    rules[4] = rule(A, 5, BRAILLE_CLASS_LETTER);
    rules[4].chords[1] = B; rules[4].chords[2] = C; rules[4].chords[3] = D;
    rules[4].chord_count = 4;
    setup(5);
    chord(A); chord(B); chord(C);
    assert(!output_count && braille_engine_pending_count() == 3);
    chord(X);
    assert(output_count == 4 && output[0] == 1 && output[1] == 2 && output[2] == 3 && output[3] == 4);
    assert(!drops && !braille_engine_pending_count());
    context_is((struct braille_context){false, BRAILLE_CAPS_NONE, false});
    puts("ok: complete fallback tail is reprocessed once in its newly committed context");
}

static void test_limits_resets_and_dispatch_failure(void) {
    rules[0] = rule(A, 1, BRAILLE_CLASS_LETTER);
    rules[0].chord_count = BRAILLE_RULE_MAX_CHORDS;
    for (unsigned i = 0; i < BRAILLE_RULE_MAX_CHORDS; ++i) rules[0].chords[i] = A;
    setup(1);
    for (unsigned i = 1; i <= BRAILLE_RULE_MAX_CHORDS; ++i) {
        chord(A);
        assert(braille_engine_pending_count() == (i == BRAILLE_RULE_MAX_CHORDS ? 0 : i));
    }
    assert(output_count == 1);
    /* An eight-cell END candidate can still wait: ninth-cell lookahead is never stored. */
    rules[0].word_position = BRAILLE_WORD_POSITION_END;
    setup(1);
    for (unsigned i = 0; i < BRAILLE_RULE_MAX_CHORDS; ++i) chord(A);
    assert(braille_engine_pending_count() == 8 && !output_count);
    chord(SPACE);
    assert(output_count == 2 && !braille_engine_pending_count());
    assert(braille_engine_set_pending_limit(0) == -EINVAL);
    assert(braille_engine_set_pending_limit(9) == -EINVAL);
    assert(braille_engine_set_pending_limit(4) == 0);
    chord(A);
    assert(!braille_engine_pending_count() && drops == 1); /* over-limit rules are not candidates */

    rules[0] = rule(A, 1, BRAILLE_CLASS_DIGIT);
    rules[1] = rule(A, 2, BRAILLE_CLASS_LETTER);
    rules[1].chords[1] = B; rules[1].chord_count = 2;
    setup(2);
    chord(A);
    seed((struct braille_context){true, BRAILLE_CAPS_PASSAGE, false});
    braille_input_reset();
    assert(!output_count && !drops && discards == 1 && !braille_engine_pending_count());
    context_is(BRAILLE_CONTEXT_INIT);
    setup(2);
    reset_on_commit = true;
    chord(A); chord(X);
    assert(output_count == 1 && !braille_engine_pending_count());
    context_is(BRAILLE_CONTEXT_INIT); /* action reset wins; old transition/tail never run */
    setup(2);
    fail_on_commit = true;
    chord(A);
    assert(braille_chord_resolved(X, ev) == -EIO);
    assert(output_count == 1 && !braille_engine_pending_count());
    context_is(BRAILLE_CONTEXT_INIT);
    puts("ok: eight-cell bound/lookahead, silent reset and no stale state after reentry/failure");
}

static void test_whitespace_and_other_command_context(void) {
    setup(0);
    static const uint16_t ws[] = {SPACE, ENTER, TAB, 0x80, 0x180};
    for (size_t i = 0; i < COUNT(ws); ++i) {
        seed((struct braille_context){true, BRAILLE_CAPS_PASSAGE, false});
        chord(ws[i]);
        context_is((struct braille_context){false, BRAILLE_CAPS_PASSAGE, true});
    }
    seed((struct braille_context){true, BRAILLE_CAPS_PASSAGE, false});
    chord(NAV);
    context_is(BRAILLE_CONTEXT_INIT);
    puts("ok: whitespace preserves passages, other commands reset context");
}

int main(void) {
    test_every_class_transition();
    test_positions_and_boundaries();
    test_full_tail_reprocessing_and_context();
    test_limits_resets_and_dispatch_failure();
    test_whitespace_and_other_command_context();
    puts("all context/pending lifecycle tests passed");
    return 0;
}

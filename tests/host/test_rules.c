// SPDX-License-Identifier: MIT
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <braille_chord.h>
#include <braille_rules.h>
#include <zmk/behavior.h>

/* Fixture factory table: read-only static const, small but exercising every
 * part of the schema. Behavior devices are stub strings; dispatch is the
 * action leaf's concern. */
static const struct braille_rule fixture_factory[] = {
    /* Single-chord lowercase letter, no predicate, any word position. */
    {
        .chords = {0x001}, /* dot 1: "a" */
        .chord_count = 1,
        .actions = {{.behavior_dev = "&kp", .param1 = 0x04, .param2 = 0}},
        .action_count = 1,
        .word_position = BRAILLE_WORD_POSITION_ANY,
        .when = {BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY},
        .class = BRAILLE_CLASS_LETTER,
        .sets = BRAILLE_CAPS_NONE,
    },
    /* The same "a" cell read as digit 1 in numeric mode. */
    {
        .chords = {0x001},
        .chord_count = 1,
        .actions = {{.behavior_dev = "&kp", .param1 = 0x1e, .param2 = 0}},
        .action_count = 1,
        .when = {.numeric = BRAILLE_CONDITION_SET},
        .class = BRAILLE_CLASS_DIGIT,
    },
    /* ⠦ as an opening quote at word start... */
    {
        .chords = {0x026}, /* dots 2, 3, 6 */
        .chord_count = 1,
        .actions = {{.behavior_dev = "&kp", .param1 = 0x34, .param2 = 0}},
        .action_count = 1,
        .word_position = BRAILLE_WORD_POSITION_START,
        .when = {.word_start = BRAILLE_CONDITION_SET},
        .class = BRAILLE_CLASS_OPENING,
    },
    /* ...and as a question mark elsewhere: same cells, disjoint predicate. */
    {
        .chords = {0x026},
        .chord_count = 1,
        .actions = {{.behavior_dev = "&kp", .param1 = 0x38, .param2 = 0}},
        .action_count = 1,
        .when = {.word_start = BRAILLE_CONDITION_CLEAR},
        .class = BRAILLE_CLASS_OTHER,
    },
    /* ⠠⠠ caps word indicator: two chords, no host output, sets word mode. */
    {
        .chords = {0x020, 0x020}, /* dot 6, dot 6 */
        .chord_count = 2,
        .actions = {0},
        .action_count = 0,
        .class = BRAILLE_CLASS_CAPS_INDICATOR,
        .sets = BRAILLE_CAPS_WORD,
    },
    /* ⠠⠠⠠ caps passage indicator: three chords, sets passage mode. */
    {
        .chords = {0x020, 0x020, 0x020},
        .chord_count = 3,
        .action_count = 0,
        .class = BRAILLE_CLASS_CAPS_INDICATOR,
        .sets = BRAILLE_CAPS_PASSAGE,
    },
    /* Uppercase letter selected by the caps predicate, two ordered actions:
     * sticky shift then the letter key, with a second parameter in use. */
    {
        .chords = {0x001},
        .chord_count = 1,
        .actions =
            {
                {.behavior_dev = "&sk", .param1 = 0xe1, .param2 = 0},
                {.behavior_dev = "&kp", .param1 = 0x04, .param2 = 0x02},
            },
        .action_count = 2,
        .when = {.caps = BRAILLE_CONDITION_SET},
        .class = BRAILLE_CLASS_LETTER,
    },
    /* Braille space: space-only mask, word-end position constraint. */
    {
        .chords = {BRAILLE_CHORD_SPACE_BIT},
        .chord_count = 1,
        .actions = {{.behavior_dev = "&kp", .param1 = 0x2c, .param2 = 0}},
        .action_count = 1,
        .word_position = BRAILLE_WORD_POSITION_END,
        .class = BRAILLE_CLASS_SPACE,
    },
    /* Maximum-size rule: 8 chords, 16 ordered actions. */
    {
        .chords = {0x001, 0x002, 0x004, 0x008, 0x010, 0x020, 0x040, 0x1ff},
        .chord_count = BRAILLE_RULE_MAX_CHORDS,
        .actions =
            {
                {.behavior_dev = "&kp", .param1 = 1, .param2 = 1},
                {.behavior_dev = "&kp", .param1 = 2, .param2 = 2},
                {.behavior_dev = "&kp", .param1 = 3, .param2 = 3},
                {.behavior_dev = "&kp", .param1 = 4, .param2 = 4},
                {.behavior_dev = "&kp", .param1 = 5, .param2 = 5},
                {.behavior_dev = "&kp", .param1 = 6, .param2 = 6},
                {.behavior_dev = "&kp", .param1 = 7, .param2 = 7},
                {.behavior_dev = "&kp", .param1 = 8, .param2 = 8},
                {.behavior_dev = "&kp", .param1 = 9, .param2 = 9},
                {.behavior_dev = "&kp", .param1 = 10, .param2 = 10},
                {.behavior_dev = "&kp", .param1 = 11, .param2 = 11},
                {.behavior_dev = "&kp", .param1 = 12, .param2 = 12},
                {.behavior_dev = "&kp", .param1 = 13, .param2 = 13},
                {.behavior_dev = "&kp", .param1 = 14, .param2 = 14},
                {.behavior_dev = "&kp", .param1 = 15, .param2 = 15},
                {.behavior_dev = "&kp", .param1 = 16, .param2 = 16},
            },
        .action_count = BRAILLE_RULE_MAX_ACTIONS,
        .word_position = BRAILLE_WORD_POSITION_MIDDLE,
        .class = BRAILLE_CLASS_NEUTRAL,
    },
};

/* The override layer: initially empty, mutable in memory, same shape. */
static struct braille_rule override_storage[4];
static size_t override_count;

static void test_schema_limits(void) {
    assert(BRAILLE_RULE_MAX_CHORDS == 8);
    assert(BRAILLE_RULE_MAX_ACTIONS == 16);
    /* Nine-bit masks fit the chord storage. */
    assert(BRAILLE_CHORD_MASK <= UINT16_MAX);
    assert(fixture_factory[8].chord_count == BRAILLE_RULE_MAX_CHORDS);
    assert(fixture_factory[8].action_count == BRAILLE_RULE_MAX_ACTIONS);
    assert(fixture_factory[8].chords[7] == BRAILLE_CHORD_MASK);
    /* Space-only, dot-only, and space-plus-dot masks share one field. */
    assert(fixture_factory[7].chords[0] == BRAILLE_CHORD_SPACE_BIT);
}

static void test_context_reset_state(void) {
    struct braille_context ctx = BRAILLE_CONTEXT_INIT;
    assert(!ctx.numeric);
    assert(ctx.caps == BRAILLE_CAPS_NONE);
    assert(ctx.word_start);
}

static void test_when_truth_table(void) {
    static const enum braille_condition conditions[] = {
        BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_SET, BRAILLE_CONDITION_CLEAR};
    static const enum braille_caps_mode caps_modes[] = {
        BRAILLE_CAPS_NONE, BRAILLE_CAPS_WORD, BRAILLE_CAPS_PASSAGE};

    for (size_t wn = 0; wn < 3; ++wn) {
        for (size_t wc = 0; wc < 3; ++wc) {
            for (size_t ww = 0; ww < 3; ++ww) {
                struct braille_rule_when when = {
                    .numeric = conditions[wn],
                    .caps = conditions[wc],
                    .word_start = conditions[ww],
                };
                for (int numeric = 0; numeric < 2; ++numeric) {
                    for (size_t caps = 0; caps < 3; ++caps) {
                        for (int word_start = 0; word_start < 2; ++word_start) {
                            struct braille_context ctx = {
                                .numeric = numeric,
                                .caps = caps_modes[caps],
                                .word_start = word_start,
                            };
                            bool expected =
                                (when.numeric == BRAILLE_CONDITION_ANY ||
                                 (when.numeric == BRAILLE_CONDITION_SET) == (bool)numeric) &&
                                (when.caps == BRAILLE_CONDITION_ANY ||
                                 (when.caps == BRAILLE_CONDITION_SET) ==
                                     (caps_modes[caps] != BRAILLE_CAPS_NONE)) &&
                                (when.word_start == BRAILLE_CONDITION_ANY ||
                                 (when.word_start == BRAILLE_CONDITION_SET) == (bool)word_start);
                            assert(braille_rule_when_holds(&when, &ctx) == expected);
                        }
                    }
                }
            }
        }
    }
}

static void test_disjoint_predicates_same_cells(void) {
    /* The two ⠦ fixture rows share cells; exactly one holds in any context. */
    const struct braille_rule *quote = &fixture_factory[2];
    const struct braille_rule *question = &fixture_factory[3];
    assert(quote->chords[0] == question->chords[0]);
    for (int word_start = 0; word_start < 2; ++word_start) {
        struct braille_context ctx = BRAILLE_CONTEXT_INIT;
        ctx.word_start = word_start;
        assert(braille_rule_when_holds(&quote->when, &ctx) !=
               braille_rule_when_holds(&question->when, &ctx));
        /* The word-start constraint is the word_start field. */
        assert(braille_rule_when_holds(&quote->when, &ctx) == (bool)word_start);
    }
}

static void test_caps_indicator_sets(void) {
    bool saw_word = false, saw_passage = false;
    for (size_t i = 0; i < sizeof(fixture_factory) / sizeof(fixture_factory[0]); ++i) {
        const struct braille_rule *rule = &fixture_factory[i];
        if (rule->class == BRAILLE_CLASS_CAPS_INDICATOR) {
            assert(rule->sets != BRAILLE_CAPS_NONE);
            saw_word |= rule->sets == BRAILLE_CAPS_WORD;
            saw_passage |= rule->sets == BRAILLE_CAPS_PASSAGE;
            assert(rule->action_count == 0); /* Indicators may emit nothing. */
        } else {
            assert(rule->sets == BRAILLE_CAPS_NONE);
        }
    }
    assert(saw_word && saw_passage);
}

static void test_uniform_output_and_table_views(void) {
    /* One output representation for characters, commands, and overrides. */
    for (size_t i = 0; i < sizeof(fixture_factory) / sizeof(fixture_factory[0]); ++i) {
        const struct braille_rule *rule = &fixture_factory[i];
        for (size_t a = 0; a < rule->action_count; ++a) {
            assert(rule->actions[a].behavior_dev != NULL);
        }
    }

    /* Read-only factory and mutable override storage share one view shape. */
    struct braille_rule_table factory = {
        .rules = fixture_factory,
        .count = sizeof(fixture_factory) / sizeof(fixture_factory[0]),
    };
    struct braille_rule_table overrides = {.rules = override_storage, .count = override_count};
    assert(factory.count == 9);
    assert(overrides.count == 0); /* Initially empty override layer. */

    /* An override is a plain rule: neutral class, no predicate, any context. */
    override_storage[0] = (struct braille_rule){
        .chords = {0x101},
        .chord_count = 1,
        .actions = {{.behavior_dev = "&kp", .param1 = 0x2c, .param2 = 0}},
        .action_count = 1,
        .class = BRAILLE_CLASS_NEUTRAL,
    };
    override_count = 1;
    overrides.count = override_count;
    struct braille_context ctx = BRAILLE_CONTEXT_INIT;
    assert(braille_rule_when_holds(&overrides.rules[0].when, &ctx));
    ctx.numeric = true;
    ctx.caps = BRAILLE_CAPS_PASSAGE;
    ctx.word_start = false;
    assert(braille_rule_when_holds(&overrides.rules[0].when, &ctx));
}

int main(void) {
    test_schema_limits();
    test_context_reset_state();
    test_when_truth_table();
    test_disjoint_predicates_same_cells();
    test_caps_indicator_sets();
    test_uniform_output_and_table_views();
    return 0;
}

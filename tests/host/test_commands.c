// SPDX-License-Identifier: MIT
/* Real capture -> matching -> dispatch, with only the ZMK invocation recorded.
 * Expected usages are independent of the production keycode macros. */
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <braille_chord.h>
#include <braille_engine.h>
#include <braille_settings.h>
#include <braille_tables.h>

#define KEY(id) ((0x07UL << 16) | (id))
#define CONSUMER(id) ((0x0cUL << 16) | (id))

struct expected_command {
    uint16_t chord;
    uint32_t key;
    enum braille_rule_class class;
    const char *behavior;
};

/* Mac VoiceOver 103560, Navigation/Scrolling/Braille; project literal space.
 * Keep this independent of the production rule array/order. */
static const struct expected_command expected[] = {
    {0x100, KEY(0x2c), BRAILLE_CLASS_SPACE, "kp"},
    {0x101, KEY(0x50), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x108, KEY(0x4f), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x104, KEY(0x52), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x120, KEY(0x51), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x107, KEY(0x4a), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x138, KEY(0x4d), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x13c, KEY(0x4b), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x139, KEY(0x4e), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x11e, KEY(0x2b), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x111, KEY(0x28), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x103, KEY(0x29), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x119, KEY(0x2a), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x040, KEY(0x2a), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x140, KEY(0x2a), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x080, KEY(0x28), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x180, KEY(0x28), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x148, KEY(0xe1), BRAILLE_CLASS_NEUTRAL, "sk"},
    {0x144, KEY(0xe0), BRAILLE_CLASS_NEUTRAL, "sk"},
    {0x142, KEY(0xe2), BRAILLE_CLASS_NEUTRAL, "sk"},
    {0x141, KEY(0xe3), BRAILLE_CLASS_NEUTRAL, "sk"},
    {0x150, CONSUMER(0x29d), BRAILLE_CLASS_NEUTRAL, "sk"},
    {0x123, CONSUMER(0xea), BRAILLE_CLASS_NEUTRAL, "kp"},
    {0x11c, CONSUMER(0xe9), BRAILLE_CLASS_NEUTRAL, "kp"},
    /* TalkBack: Use braille keystrokes, TalkBack features. */
    {0x1c0, CONSUMER(0xcd), BRAILLE_CLASS_NEUTRAL, "kp"},
};

struct invocation {
    struct zmk_behavior_binding binding;
    struct zmk_behavior_binding_event event;
    bool pressed;
};
static struct invocation output[32];
static size_t output_count;

int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event, bool pressed) {
    assert(output_count < sizeof(output) / sizeof(output[0]));
    output[output_count++] = (struct invocation){*binding, event, pressed};
    return 0;
}

static const struct zmk_behavior_binding_event ev = {
    .layer = 3, .position = 8, .timestamp = 100,
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
    .source = 1,
#endif
};

static void chord(uint16_t mask) {
    /* One physical position per bit. No invocation is allowed until the final
     * key release, including Tab's space bit; capture is not bypassed. */
    size_t before = output_count;
    for (unsigned bit = 0; bit < 9; ++bit) {
        if (mask & (1U << bit)) {
            struct zmk_behavior_binding_event edge = ev;
            edge.position = bit;
            assert(braille_chord_key_event(1U << bit, true, edge) == 0);
            assert(output_count == before);
        }
    }
    for (unsigned bit = 0; bit < 9; ++bit) {
        if (mask & (1U << bit)) {
            struct zmk_behavior_binding_event edge = ev;
            edge.position = bit;
            assert(braille_chord_key_event(1U << bit, false, edge) == 0);
            if (mask >> (bit + 1)) {
                assert(output_count == before);
            }
        }
    }
}

static const struct braille_rule *find(const struct braille_rule_table *table, uint16_t mask) {
    for (size_t i = 0; i < table->count; ++i) {
        if (table->rules[i].chord_count == 1 && table->rules[i].chords[0] == mask) {
            return &table->rules[i];
        }
    }
    return NULL;
}

static void expect_action(uint32_t key, const char *behavior) {
    assert(output_count == 2);
    for (size_t i = 0; i < output_count; ++i) {
        assert(strcmp(output[i].binding.behavior_dev, behavior) == 0);
        assert(output[i].binding.param1 == key);
        assert(output[i].binding.param2 == 0);
        assert(output[i].pressed == (i == 0));
        assert(output[i].event.layer == ev.layer);
        assert(output[i].event.timestamp == ev.timestamp);
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
        assert(output[i].event.source == ev.source);
#endif
    }
    assert(braille_engine_pending_count() == 0);
}

static void expect_key(uint32_t key) { expect_action(key, "kp"); }

static void test_tables_and_dispatch(void) {
    for (size_t m = 0; m < braille_tables_count(); ++m) {
        const struct braille_mode *mode = braille_tables_mode(m);
        assert(braille_tables_select(mode->name) == 0);
        assert(braille_engine_validate_factory(mode->characters, mode->commands) == 0);
        assert(mode->commands->count == sizeof(expected) / sizeof(expected[0]));
        for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); ++i) {
            const struct braille_rule *rule = find(mode->commands, expected[i].chord);
            assert(rule != NULL);
            assert(rule->action_count == 1);
            assert(rule->class == expected[i].class);
            assert(rule->word_position == BRAILLE_WORD_POSITION_ANY);
            assert(rule->when.numeric == BRAILLE_CONDITION_ANY);
            assert(rule->when.caps == BRAILLE_CONDITION_ANY);
            assert(rule->when.word_start == BRAILLE_CONDITION_ANY);
            output_count = 0;
            chord(expected[i].chord);
            expect_action(expected[i].key, expected[i].behavior);
            assert(braille_engine_is_whitespace(expected[i].chord) ==
                   (expected[i].chord == 0x100 || expected[i].chord == 0x111 ||
                    expected[i].chord == 0x11e || expected[i].chord == 0x080 ||
                    expected[i].chord == 0x180));
            assert(!braille_engine_is_whitespace(0));
        }
    }
    puts("ok: every verified factory command dispatches in each compiled mode");
}

static void test_exact_tab_not_alphabet(void) {
    for (size_t m = 0; m < braille_tables_count(); ++m) {
        assert(braille_tables_select(braille_tables_mode(m)->name) == 0);
        output_count = 0;
        chord(0x11e); /* space + 2345 -> Tab ONLY, never 't'. */
        expect_key(KEY(0x2b));
        assert(output[0].event.position == 8); /* last release: Braille space */
        assert(output[1].event.position == 8);
        output_count = 0;
        chord(0x118); /* removed fixture: space + 45 is VoiceOver Translate. */
        assert(output_count == 0);
        assert(!braille_engine_is_whitespace(0x118));
        assert(braille_engine_pending_count() == 0);
    }
    assert(braille_tables_select("ueb1") == 0);
    output_count = 0;
    chord(0x01e); /* same dots WITHOUT space are the alphabet's 't'. */
    expect_key(KEY(0x17));
    puts("ok: exact 0x11e Tab is isolated from alphabet and removed fixture");
}

static void test_fixture_characters_and_collision_rejection(void) {
    static const struct braille_rule fixture_rules[] = {
        {.chords = {0x01}, .chord_count = 1,
         .actions = {{.behavior_dev = "kp", .param1 = KEY(0x04)}}, .action_count = 1},
    };
    const struct braille_rule_table fixture = {.rules = fixture_rules, .count = 1};
    for (size_t m = 0; m < braille_tables_count(); ++m) {
        const struct braille_mode *mode = braille_tables_mode(m);
        assert(braille_engine_set_tables(&fixture, mode->commands) == 0);
        braille_engine_set_whitespace(mode->whitespace);
        output_count = 0;
        chord(0x01);
        expect_key(KEY(0x04));
        output_count = 0;
        chord(0x11e);
        expect_key(KEY(0x2b));

        struct braille_rule colliding = fixture_rules[0];
        colliding.chords[0] = 0x11e;
        const struct braille_rule_table bad = {.rules = &colliding, .count = 1};
        assert(braille_engine_set_tables(&bad, mode->commands) == -EINVAL);
        /* Invalid selection preserves the previously selected fixture. */
        output_count = 0;
        chord(0x01);
        expect_key(KEY(0x04));
    }
    puts("ok: fixture character coexistence and cross-table collision rejection");
}

static void test_return_aliases_commit_pending_text(void) {
    static const struct braille_rule character = {
        .chords = {0x01}, .chord_count = 1,
        .actions = {{.behavior_dev = "kp", .param1 = KEY(0x04)}}, .action_count = 1,
    };
    static const struct braille_rule longer = {
        .chords = {0x01, 0x01}, .chord_count = 2,
        .actions = {{.behavior_dev = "kp", .param1 = KEY(0x05)}}, .action_count = 1,
    };
    const struct braille_rule_table fixture = {.rules = &character, .count = 1};
    const struct braille_rule_table overrides = {.rules = &longer, .count = 1};
    static const uint16_t returns[] = {0x111, 0x080, 0x180};
    for (size_t m = 0; m < braille_tables_count(); ++m) {
        const struct braille_mode *mode = braille_tables_mode(m);
        for (size_t r = 0; r < sizeof(returns) / sizeof(returns[0]); ++r) {
            assert(braille_engine_set_tables(&fixture, mode->commands) == 0);
            braille_engine_set_whitespace(mode->whitespace);
            braille_engine_set_overrides(&overrides);
            output_count = 0;
            chord(0x01);
            assert(output_count == 0 && braille_engine_pending_count() == 1);
            chord(returns[r]);
            assert(output_count == 4);
            assert(output[0].binding.param1 == KEY(0x04) && output[0].pressed);
            assert(output[1].binding.param1 == KEY(0x04) && !output[1].pressed);
            assert(output[2].binding.param1 == KEY(0x28) && output[2].pressed);
            assert(output[3].binding.param1 == KEY(0x28) && !output[3].pressed);
            assert(braille_engine_pending_count() == 0);
        }
    }
    braille_engine_set_overrides(NULL);
    puts("ok: every Return alias preserves the whitespace pending-text path");
}

int main(void) {
    test_tables_and_dispatch();
    test_exact_tab_not_alphabet();
    test_fixture_characters_and_collision_rejection();
    test_return_aliases_commit_pending_text();
    puts("all factory command tests passed");
    return 0;
}

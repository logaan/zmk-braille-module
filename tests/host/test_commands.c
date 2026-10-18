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

struct expected_command {
    uint16_t chord;
    uint32_t key;
    enum braille_rule_class class;
};

/* Mac VoiceOver 103560, Navigation/Scrolling/Braille; project literal space.
 * Keep this independent of the production rule array/order. */
static const struct expected_command expected[] = {
    {0x100, KEY(0x2c), BRAILLE_CLASS_SPACE},
    {0x101, KEY(0x50), BRAILLE_CLASS_NEUTRAL},
    {0x108, KEY(0x4f), BRAILLE_CLASS_NEUTRAL},
    {0x104, KEY(0x52), BRAILLE_CLASS_NEUTRAL},
    {0x120, KEY(0x51), BRAILLE_CLASS_NEUTRAL},
    {0x107, KEY(0x4a), BRAILLE_CLASS_NEUTRAL},
    {0x138, KEY(0x4d), BRAILLE_CLASS_NEUTRAL},
    {0x13c, KEY(0x4b), BRAILLE_CLASS_NEUTRAL},
    {0x139, KEY(0x4e), BRAILLE_CLASS_NEUTRAL},
    {0x11e, KEY(0x2b), BRAILLE_CLASS_NEUTRAL},
    {0x111, KEY(0x28), BRAILLE_CLASS_NEUTRAL},
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

static void expect_key(uint32_t key) {
    assert(output_count == 2);
    for (size_t i = 0; i < output_count; ++i) {
        assert(strcmp(output[i].binding.behavior_dev, "kp") == 0);
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
            expect_key(expected[i].key);
            assert(braille_engine_is_whitespace(expected[i].chord) ==
                   (expected[i].chord == 0x100 || expected[i].chord == 0x111 ||
                    expected[i].chord == 0x11e));
        }
    }
    puts("ok: every factory navigation rule dispatches in each compiled mode");
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

int main(void) {
    test_tables_and_dispatch();
    test_exact_tab_not_alphabet();
    test_fixture_characters_and_collision_rejection();
    puts("all factory command tests passed");
    return 0;
}

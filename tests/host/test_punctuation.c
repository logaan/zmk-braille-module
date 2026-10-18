// SPDX-License-Identifier: MIT
/* Task 9.3.1.1: real capture -> UEB1 selection/matching -> ordered dispatch.
 * Only ZMK behavior invocation is recorded. Expected HID values deliberately
 * do not use the production keycode macros (including implicit Shift). */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <braille_chord.h>
#include <braille_engine.h>
#include <braille_tables.h>

#define KEY(id) ((0x07UL << 16) | (id))
#define SHIFTED(id) ((0x02UL << 24) | KEY(id))

struct punctuation_entry {
    uint16_t chord;
    uint32_t key;
    enum braille_rule_class class;
};

/* ICEB 2024 section 7, p.75; class choices preserve the context design from
 * 2.6.1/2.6.2 and 6.2.1 for the future phase-04 transitions. */
static const struct punctuation_entry punctuation[] = {
    {0x02, KEY(0x36), BRAILLE_CLASS_NUMERIC_PUNCTUATION}, /* 2: comma */
    {0x06, KEY(0x33), BRAILLE_CLASS_OTHER},               /* 23: semicolon */
    {0x12, SHIFTED(0x33), BRAILLE_CLASS_OTHER},           /* 25: colon */
    {0x16, SHIFTED(0x1e), BRAILLE_CLASS_OTHER},           /* 235: exclamation */
    {0x04, KEY(0x34), BRAILLE_CLASS_OPENING},             /* 3: apostrophe */
    {0x24, KEY(0x2d), BRAILLE_CLASS_HYPHEN},              /* 36: hyphen */
    {0x32, KEY(0x37), BRAILLE_CLASS_NUMERIC_PUNCTUATION}, /* 256: full stop */
};

struct invocation {
    struct zmk_behavior_binding binding;
    struct zmk_behavior_binding_event event;
    bool pressed;
};
static struct invocation output[64];
static size_t output_count;

int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event, bool pressed) {
    assert(output_count < sizeof(output) / sizeof(output[0]));
    output[output_count++] = (struct invocation){*binding, event, pressed};
    return 0;
}

static const struct zmk_behavior_binding_event ev = {
    .layer = 2, .position = 0, .timestamp = 123,
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
    .source = 1,
#endif
};

static unsigned last_bit(uint16_t mask) {
    unsigned bit = 0;
    while (mask >>= 1) {
        ++bit;
    }
    return bit;
}

static void chord(uint16_t mask) {
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
            if (bit != last_bit(mask)) {
                assert(output_count == before); /* release-terminated, not timed */
            }
        }
    }
    assert(braille_engine_pending_count() == 0);
}

static void expect_pair(size_t offset, uint32_t key, uint16_t mask) {
    assert(output_count >= offset + 2);
    for (size_t i = 0; i < 2; ++i) {
        const struct invocation *out = &output[offset + i];
        assert(strcmp(out->binding.behavior_dev, "kp") == 0);
        assert(out->binding.param1 == key);
        assert(out->binding.param2 == 0);
        assert(out->pressed == (i == 0));
        assert(out->event.layer == ev.layer);
        assert(out->event.position == last_bit(mask));
        assert(out->event.timestamp == ev.timestamp);
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
        assert(out->event.source == ev.source);
#endif
    }
}

static void test_rules_and_real_table_validation(void) {
    const struct braille_mode *mode = braille_tables_find("ueb1");
    assert(mode != NULL);
    assert(mode->characters == &braille_rules_ueb1_characters);
    assert(mode->commands == &braille_rules_ueb1_commands);
    assert(braille_engine_validate_factory(mode->characters, mode->commands) == 0);
    assert(braille_tables_select("ueb1") == 0);
    for (size_t p = 0; p < sizeof(punctuation) / sizeof(punctuation[0]); ++p) {
        const struct braille_rule *found = NULL;
        for (size_t r = 0; r < mode->characters->count; ++r) {
            const struct braille_rule *rule = &mode->characters->rules[r];
            if (rule->chord_count == 1 && rule->chords[0] == punctuation[p].chord) {
                assert(found == NULL); /* no duplicate/conditional variants */
                found = rule;
            }
        }
        assert(found != NULL);
        assert(found->action_count == 1);
        assert(strcmp(found->actions[0].behavior_dev, "kp") == 0);
        assert(found->actions[0].param1 == punctuation[p].key);
        assert(found->actions[0].param2 == 0);
        assert(found->class == punctuation[p].class);
        assert(found->word_position == BRAILLE_WORD_POSITION_ANY);
        assert(found->when.numeric == BRAILLE_CONDITION_ANY);
        assert(found->when.caps == BRAILLE_CONDITION_ANY);
        assert(found->when.word_start == BRAILLE_CONDITION_ANY);
        assert(found->sets == BRAILLE_CAPS_NONE);
        assert(!braille_engine_is_whitespace(punctuation[p].chord));
        for (unsigned numeric = 0; numeric < 2; ++numeric) {
            for (int caps = BRAILLE_CAPS_NONE; caps <= BRAILLE_CAPS_PASSAGE; ++caps) {
                for (unsigned word_start = 0; word_start < 2; ++word_start) {
                    struct braille_context ctx = {numeric, caps, word_start};
                    assert(braille_rule_when_holds(&found->when, &ctx));
                }
            }
        }
    }
    puts("ok: seven unconditional rules retain class metadata and validate with real commands");
}

static void test_every_sign_at_reset_and_after_letter(void) {
    for (size_t p = 0; p < sizeof(punctuation) / sizeof(punctuation[0]); ++p) {
        for (unsigned after_letter = 0; after_letter < 2; ++after_letter) {
            braille_engine_reset();
            output_count = 0;
            if (after_letter) {
                chord(0x01); /* a, as in the existing YAML acceptance cases */
                assert(output_count == 2);
                expect_pair(0, KEY(0x04), 0x01);
                output_count = 0;
            }
            chord(punctuation[p].chord);
            assert(output_count == 2);
            expect_pair(0, punctuation[p].key, punctuation[p].chord);
        }
    }
    puts("ok: every sign dispatches one complete host keystroke at reset and after a letter");
}

static void test_alphabet_punctuation_and_commands_in_one_stream(void) {
    static const struct {
        uint16_t chord;
        uint32_t key;
    } stream[] = {
        {0x01, KEY(0x04)},   /* a */
        {0x02, KEY(0x36)},   /* , */
        {0x03, KEY(0x05)},   /* b */
        {0x06, KEY(0x33)},   /* ; */
        {0x12, SHIFTED(0x33)}, /* : */
        {0x16, SHIFTED(0x1e)}, /* ! */
        {0x04, KEY(0x34)},   /* ' */
        {0x24, KEY(0x2d)},   /* - */
        {0x32, KEY(0x37)},   /* . */
        {0x100, KEY(0x2c)},  /* Braille space */
        {0x11e, KEY(0x2b)},  /* Tab: must never emit t */
        {0x111, KEY(0x28)},  /* Return */
        {0x101, KEY(0x50)},  /* Left */
        {0x040, KEY(0x2a)},  /* dot-7 Delete */
        {0x080, KEY(0x28)},  /* dot-8 Return */
        {0x32, KEY(0x37)},   /* punctuation still works after commands */
        {0x35, KEY(0x1d)},   /* z */
    };
    braille_engine_reset();
    output_count = 0;
    for (size_t i = 0; i < sizeof(stream) / sizeof(stream[0]); ++i) {
        chord(stream[i].chord);
        assert(output_count == 2 * (i + 1));
        expect_pair(2 * i, stream[i].key, stream[i].chord);
    }
    puts("ok: alphabet, all seven signs and phase-02 commands coexist in order");
}

static void test_dots_236_remain_deferred(void) {
    braille_engine_reset();
    output_count = 0;
    chord(0x26); /* at reset: no premature opening-quote policy */
    assert(output_count == 0);
    chord(0x01); /* a */
    assert(output_count == 2);
    expect_pair(0, KEY(0x04), 0x01);
    output_count = 0;
    chord(0x26); /* after a letter: no premature question-mark policy */
    assert(output_count == 0);
    chord(0x03); /* no pending cells or lost next letter */
    assert(output_count == 2);
    expect_pair(0, KEY(0x05), 0x03);
    puts("ok: dots 2,3,6 stay unassigned; phase-04 question/quote acceptance fixtures remain intact");
}

int main(void) {
    test_rules_and_real_table_validation();
    test_every_sign_at_reset_and_after_letter();
    test_alphabet_punctuation_and_commands_in_one_stream();
    test_dots_236_remain_deferred();
    puts("all core punctuation tests passed");
    return 0;
}

// SPDX-License-Identifier: MIT
/* Task 9.3.1.2: production capture -> real UEB1/command tables -> matcher ->
 * ordered dispatch. Only ZMK behavior invocation and the display notice are
 * recorded. Expected host usages (including Shift) are independent of the
 * production keycode macros. */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <braille_chord.h>
#include <braille_engine.h>
#include <braille_tables.h>

#define KEY(id) ((0x07UL << 16) | (id))
#define SHIFTED(id) ((0x02UL << 24) | KEY(id))
#define COUNT(a) (sizeof(a) / sizeof((a)[0]))

struct symbol_entry {
    char print;
    uint16_t first;
    uint16_t second;
    uint32_t key;
    enum braille_rule_class class;
};

/* The task's exact ordered sequences; ( [ { alone are 2.6.2 openers. */
static const struct symbol_entry symbols[] = {
    {'&', 0x08, 0x2f, SHIFTED(0x24), BRAILLE_CLASS_OTHER},
    {'*', 0x10, 0x14, SHIFTED(0x25), BRAILLE_CLASS_OTHER},
    {'@', 0x08, 0x01, SHIFTED(0x1f), BRAILLE_CLASS_OTHER},
    {'^', 0x08, 0x22, SHIFTED(0x23), BRAILLE_CLASS_OTHER},
    {'#', 0x38, 0x39, SHIFTED(0x20), BRAILLE_CLASS_OTHER},
    {'$', 0x08, 0x0e, SHIFTED(0x21), BRAILLE_CLASS_OTHER},
    {'%', 0x28, 0x34, SHIFTED(0x22), BRAILLE_CLASS_OTHER},
    {'~', 0x08, 0x14, SHIFTED(0x35), BRAILLE_CLASS_OTHER},
    {'/', 0x38, 0x0c, KEY(0x38), BRAILLE_CLASS_OTHER},
    {'_', 0x28, 0x24, SHIFTED(0x2d), BRAILLE_CLASS_OTHER},
    {'+', 0x10, 0x16, SHIFTED(0x2e), BRAILLE_CLASS_OTHER},
    {'=', 0x10, 0x36, KEY(0x2e), BRAILLE_CLASS_OTHER},
    {'<', 0x08, 0x23, SHIFTED(0x36), BRAILLE_CLASS_OTHER},
    {'>', 0x08, 0x1c, SHIFTED(0x37), BRAILLE_CLASS_OTHER},
    {'(', 0x10, 0x23, SHIFTED(0x26), BRAILLE_CLASS_OPENING},
    {')', 0x10, 0x1c, SHIFTED(0x27), BRAILLE_CLASS_OTHER},
    {'[', 0x28, 0x23, KEY(0x2f), BRAILLE_CLASS_OPENING},
    {']', 0x28, 0x1c, KEY(0x30), BRAILLE_CLASS_OTHER},
    {'{', 0x38, 0x23, SHIFTED(0x2f), BRAILLE_CLASS_OPENING},
    {'}', 0x38, 0x1c, SHIFTED(0x30), BRAILLE_CLASS_OTHER},
    {'\\', 0x38, 0x21, KEY(0x31), BRAILLE_CLASS_OTHER},
};

struct invocation {
    struct zmk_behavior_binding binding;
    struct zmk_behavior_binding_event event;
    bool pressed;
};
static struct invocation output[64];
static size_t output_count;
static struct {
    uint16_t cells[BRAILLE_RULE_MAX_CHORDS];
    uint8_t count;
    struct zmk_behavior_binding_event event;
    size_t output_count;
} notices[8];
static size_t notice_count;
static int64_t timestamp;

int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event, bool pressed) {
    assert(output_count < COUNT(output));
    output[output_count++] = (struct invocation){*binding, event, pressed};
    return 0;
}

void braille_engine_notify_dropped_prefix(const uint16_t *cells, uint8_t count,
                                          struct zmk_behavior_binding_event event) {
    assert(notice_count < COUNT(notices));
    assert(count <= BRAILLE_RULE_MAX_CHORDS);
    memcpy(notices[notice_count].cells, cells, count * sizeof(*cells));
    notices[notice_count].count = count;
    notices[notice_count].event = event;
    notices[notice_count].output_count = output_count;
    ++notice_count;
}

static void reset(void) {
    braille_engine_reset();
    output_count = 0;
    notice_count = 0;
}

static unsigned last_bit(uint16_t mask) {
    unsigned bit = 0;
    while (mask >>= 1) {
        ++bit;
    }
    return bit;
}

/* Exercise real press/release capture and return the last-release event. */
static struct zmk_behavior_binding_event chord(uint16_t mask) {
    size_t before = output_count;
    struct zmk_behavior_binding_event event = {
        .layer = 2, .timestamp = ++timestamp,
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
        .source = 1,
#endif
    };
    assert(mask > 0 && mask <= BRAILLE_CHORD_MASK);
    for (unsigned bit = 0; bit < 9; ++bit) {
        if (mask & (1U << bit)) {
            event.position = bit;
            assert(braille_chord_key_event(1U << bit, true, event) == 0);
            assert(output_count == before);
        }
    }
    for (unsigned bit = 0; bit < 9; ++bit) {
        if (mask & (1U << bit)) {
            event.position = bit;
            assert(braille_chord_key_event(1U << bit, false, event) == 0);
            if (bit != last_bit(mask)) {
                assert(output_count == before);
            }
        }
    }
    return event;
}

static void expect_event(struct zmk_behavior_binding_event actual,
                         struct zmk_behavior_binding_event expected) {
    assert(actual.layer == expected.layer);
    assert(actual.position == expected.position);
    assert(actual.timestamp == expected.timestamp);
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
    assert(actual.source == expected.source);
#endif
}

static void expect_pair(size_t offset, const char *behavior, uint32_t key,
                        struct zmk_behavior_binding_event event) {
    assert(output_count >= offset + 2);
    for (size_t i = 0; i < 2; ++i) {
        const struct invocation *out = &output[offset + i];
        assert(strcmp(out->binding.behavior_dev, behavior) == 0);
        assert(out->binding.param1 == key);
        assert(out->binding.param2 == 0);
        assert(out->pressed == (i == 0));
        expect_event(out->event, event);
    }
}

static void expect_dropped(uint16_t prefix, struct zmk_behavior_binding_event event) {
    assert(notice_count == 1);
    assert(notices[0].count == 1);
    assert(notices[0].cells[0] == prefix);
    assert(notices[0].output_count == 0); /* notice precedes following host key */
    expect_event(notices[0].event, event);
}

static void test_rule_data_and_real_commands(void) {
    const struct braille_mode *mode = braille_tables_find("ueb1");
    assert(mode != NULL);
    assert(mode->characters == &braille_rules_ueb1_characters);
    assert(mode->commands == &braille_rules_ueb1_commands);
    assert(braille_engine_validate_factory(mode->characters, mode->commands) == 0);
    assert(braille_tables_select("ueb1") == 0);
    assert(COUNT(symbols) == 21);
    for (size_t s = 0; s < COUNT(symbols); ++s) {
        const struct symbol_entry *symbol = &symbols[s];
        const struct braille_rule *found = NULL;
        for (size_t r = 0; r < mode->characters->count; ++r) {
            const struct braille_rule *rule = &mode->characters->rules[r];
            /* The first cell must remain a strict prefix, never an action. */
            assert(rule->chord_count != 1 || rule->chords[0] != symbol->first);
            if (rule->chord_count == 2 && rule->chords[0] == symbol->first &&
                rule->chords[1] == symbol->second) {
                assert(found == NULL);
                found = rule;
            }
        }
        assert(found != NULL);
        assert(found->action_count == 1);
        assert(strcmp(found->actions[0].behavior_dev, "kp") == 0);
        assert(found->actions[0].param1 == symbol->key);
        assert(found->actions[0].param2 == 0);
        assert(found->class == symbol->class);
        assert(found->word_position == BRAILLE_WORD_POSITION_ANY);
        assert(found->when.numeric == BRAILLE_CONDITION_ANY);
        assert(found->when.caps == BRAILLE_CONDITION_ANY);
        assert(found->when.word_start == BRAILLE_CONDITION_ANY);
        assert(found->sets == BRAILLE_CAPS_NONE);
        for (unsigned numeric = 0; numeric < 2; ++numeric) {
            for (int caps = BRAILLE_CAPS_NONE; caps <= BRAILLE_CAPS_PASSAGE; ++caps) {
                for (unsigned word_start = 0; word_start < 2; ++word_start) {
                    struct braille_context context = {numeric, caps, word_start};
                    assert(braille_rule_when_holds(&found->when, &context));
                }
            }
        }
    }
    puts("ok: 21 unconditional two-cell rules and classes validate with real commands");
}

static void test_every_completed_sign(void) {
    for (size_t s = 0; s < COUNT(symbols); ++s) {
        for (unsigned after_letter = 0; after_letter < 2; ++after_letter) {
            reset();
            if (after_letter) {
                struct zmk_behavior_binding_event event = chord(0x01);
                assert(output_count == 2);
                expect_pair(0, "kp", KEY(0x04), event); /* a */
                output_count = 0;
            }
            chord(symbols[s].first);
            assert(output_count == 0 && notice_count == 0);
            assert(braille_engine_pending_count() == 1);
            struct zmk_behavior_binding_event event = chord(symbols[s].second);
            assert(output_count == 2 && notice_count == 0);
            assert(braille_engine_pending_count() == 0);
            expect_pair(0, "kp", symbols[s].key, event);
        }
    }
    puts("ok: every ordered sign waits, then emits exactly one host key at reset/after a letter");
}

static void test_reversed_order_does_not_match(void) {
    for (size_t s = 0; s < COUNT(symbols); ++s) {
        reset();
        chord(symbols[s].second);
        assert(braille_engine_pending_count() == 0);
        for (size_t i = 0; i < output_count; ++i) {
            assert(output[i].binding.param1 != symbols[s].key);
        }
        size_t before = output_count;
        chord(symbols[s].first);
        assert(output_count == before);
        assert(braille_engine_pending_count() == 1);
    }
    reset();
    puts("ok: reversing any of the 21 sequences cannot emit its symbol");
}

static void test_every_first_cell_non_continuation(void) {
    for (size_t s = 0; s < COUNT(symbols); ++s) {
        reset();
        chord(symbols[s].first);
        assert(output_count == 0 && notice_count == 0);
        assert(braille_engine_pending_count() == 1);
        /* z is not a continuation of ANY of the four first cells. */
        struct zmk_behavior_binding_event event = chord(0x35);
        expect_dropped(symbols[s].first, event);
        assert(output_count == 2 && braille_engine_pending_count() == 0);
        expect_pair(0, "kp", KEY(0x1d), event);
        event = chord(0x03); /* b is read exactly once, with no stale prefix. */
        assert(output_count == 4 && notice_count == 1);
        expect_pair(2, "kp", KEY(0x05), event);
    }
    puts("ok: each sign's first cell + non-continuing z drops only the prefix and re-reads z");
}

static void test_non_continuation_can_start_another_symbol(void) {
    static const size_t representatives[] = {0, 1, 6, 4}; /* first cells 4,5,46,456 */
    for (size_t i = 0; i < COUNT(representatives); ++i) {
        const struct symbol_entry *old = &symbols[representatives[i]];
        const struct symbol_entry *next = &symbols[representatives[(i + 1) % COUNT(representatives)]];
        reset();
        chord(old->first);
        struct zmk_behavior_binding_event event = chord(next->first);
        expect_dropped(old->first, event);
        assert(output_count == 0 && braille_engine_pending_count() == 1);
        event = chord(next->second);
        assert(output_count == 2 && notice_count == 1);
        assert(braille_engine_pending_count() == 0);
        expect_pair(0, "kp", next->key, event);
    }
    puts("ok: a non-continuing first cell starts its own fresh pending symbol");
}

static void test_prefixes_with_every_real_command(void) {
    static const uint16_t prefixes[] = {0x08, 0x10, 0x28, 0x38};
    const struct braille_rule_table *commands = braille_tables_find("ueb1")->commands;
    for (size_t p = 0; p < COUNT(prefixes); ++p) {
        for (size_t c = 0; c < commands->count; ++c) {
            const struct braille_rule *command = &commands->rules[c];
            assert(command->chord_count == 1 && command->action_count == 1);
            reset();
            chord(prefixes[p]);
            struct zmk_behavior_binding_event event = chord(command->chords[0]);
            expect_dropped(prefixes[p], event);
            assert(output_count == 2 && braille_engine_pending_count() == 0);
            expect_pair(0, command->actions[0].behavior_dev, command->actions[0].param1, event);
        }
    }
    puts("ok: all four strict prefixes recover before every real phase-02 command");
}

static void test_all_symbols_in_one_stream(void) {
    reset();
    for (size_t s = 0; s < COUNT(symbols); ++s) {
        chord(symbols[s].first);
        assert(output_count == 2 * s && braille_engine_pending_count() == 1);
        struct zmk_behavior_binding_event event = chord(symbols[s].second);
        assert(output_count == 2 * (s + 1) && braille_engine_pending_count() == 0);
        expect_pair(2 * s, "kp", symbols[s].key, event);
    }
    assert(notice_count == 0);
    puts("ok: all 21 symbols dispatch in order without resets or leaked prefix output");
}

int main(int argc, char **argv) {
    if (argc > 1) {
        /* Run scoped YAML fixtures through the same production path. */
        assert(strcmp(argv[1], "--sequence") == 0);
        assert(braille_tables_select("ueb1") == 0);
        for (int i = 2; i < argc; ++i) {
            char *end;
            unsigned long mask = strtoul(argv[i], &end, 0);
            assert(*end == '\0' && mask > 0 && mask <= BRAILLE_CHORD_MASK);
            chord((uint16_t)mask);
        }
        assert(braille_engine_pending_count() == 0 && notice_count == 0);
        assert(output_count % 2 == 0);
        for (size_t i = 0; i < output_count; i += 2) {
            expect_pair(i, "kp", output[i].binding.param1, output[i].event);
            printf("%08x\n", (unsigned)output[i].binding.param1);
        }
        return 0;
    }
    test_rule_data_and_real_commands();
    test_every_completed_sign();
    test_reversed_order_does_not_match();
    test_every_first_cell_non_continuation();
    test_non_continuation_can_start_another_symbol();
    test_prefixes_with_every_real_command();
    test_all_symbols_in_one_stream();
    puts("all specified multi-cell symbol tests passed");
    return 0;
}

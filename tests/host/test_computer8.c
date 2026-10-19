// SPDX-License-Identifier: MIT
/* Task 9.3.2.1.2: all patterns through production capture, real table
 * selection, matching and ordered dispatch. Only host invocation and the
 * future display's notification hook are recorded. */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <braille_chord.h>
#include <braille_engine.h>
#include <braille_tables.h>

#define KEY(id) (0x070000UL | (id))
#define SHIFT(id) (0x02070000UL | (id))
#define COUNT(a) (sizeof(a) / sizeof((a)[0]))

/* Independent input oracle, ASCII !..~ (original Liblouis fixtures). */
static const uint16_t ascii_masks[] = {
    0x2e, 0x10, 0x3c, 0x2b, 0x29, 0x2f, 0x04, 0x37, 0x3e, 0x21, 0x2c, 0x20, 0x24, 0x28, 0x0c,
    0x34, 0x02, 0x06, 0x12, 0x32, 0x22, 0x16, 0x36, 0x26, 0x14,
    0x31, 0x30, 0x23, 0x3f, 0x1c, 0x39, 0x48,
    0x41, 0x43, 0x49, 0x59, 0x51, 0x4b, 0x5b, 0x53, 0x4a, 0x5a, 0x45, 0x47, 0x4d,
    0x5d, 0x55, 0x4f, 0x5f, 0x57, 0x4e, 0x5e, 0x65, 0x67, 0x7a, 0x6d, 0x7d, 0x75,
    0x6a, 0x73, 0x7b, 0x58, 0x38, 0x08,
    0x01, 0x03, 0x09, 0x19, 0x11, 0x0b, 0x1b, 0x13, 0x0a, 0x1a, 0x05, 0x07, 0x0d,
    0x1d, 0x15, 0x0f, 0x1f, 0x17, 0x0e, 0x1e, 0x25, 0x27, 0x3a, 0x2d, 0x3d, 0x35,
    0x2a, 0x33, 0x3b, 0x18,
};
_Static_assert(COUNT(ascii_masks) == 94, "94 printable non-space ASCII characters");

/* Independent raw HID/US-layout oracle; no ZMK keycode macros. */
static uint32_t host_key(unsigned char c) {
    if (c >= 'a' && c <= 'z') { return KEY(0x04 + c - 'a'); }
    if (c >= 'A' && c <= 'Z') { return SHIFT(0x04 + c - 'A'); }
    if (c >= '1' && c <= '9') { return KEY(0x1e + c - '1'); }
    switch (c) {
    case '0': return KEY(0x27);
    case ' ': return KEY(0x2c);
    case '!': return SHIFT(0x1e);
    case '"': return SHIFT(0x34);
    case '#': return SHIFT(0x20);
    case '$': return SHIFT(0x21);
    case '%': return SHIFT(0x22);
    case '&': return SHIFT(0x24);
    case '\'': return KEY(0x34);
    case '(': return SHIFT(0x26);
    case ')': return SHIFT(0x27);
    case '*': return SHIFT(0x25);
    case '+': return SHIFT(0x2e);
    case ',': return KEY(0x36);
    case '-': return KEY(0x2d);
    case '.': return KEY(0x37);
    case '/': return KEY(0x38);
    case ':': return SHIFT(0x33);
    case ';': return KEY(0x33);
    case '<': return SHIFT(0x36);
    case '=': return KEY(0x2e);
    case '>': return SHIFT(0x37);
    case '?': return SHIFT(0x38);
    case '@': return SHIFT(0x1f);
    case '[': return KEY(0x2f);
    case '\\': return KEY(0x31);
    case ']': return KEY(0x30);
    case '^': return SHIFT(0x23);
    case '_': return SHIFT(0x2d);
    case '`': return KEY(0x35);
    case '{': return SHIFT(0x2f);
    case '|': return SHIFT(0x31);
    case '}': return SHIFT(0x30);
    case '~': return SHIFT(0x35);
    default: assert(false); return 0;
    }
}

struct invocation {
    struct zmk_behavior_binding binding;
    struct zmk_behavior_binding_event event;
    bool pressed;
};
static struct invocation output[512];
static size_t output_count;
static uint16_t noticed_cells[BRAILLE_RULE_MAX_CHORDS];
static uint8_t noticed_count;
static size_t notice_count;
static struct zmk_behavior_binding_event noticed_event;
static int64_t timestamp;

int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event, bool pressed) {
    assert(output_count < COUNT(output));
    output[output_count++] = (struct invocation){*binding, event, pressed};
    return 0;
}

void braille_engine_notify_dropped_prefix(const uint16_t *cells, uint8_t count,
                                          struct zmk_behavior_binding_event event) {
    assert(count <= COUNT(noticed_cells));
    memcpy(noticed_cells, cells, count * sizeof(*cells));
    noticed_count = count;
    noticed_event = event;
    ++notice_count;
}

static void reset(void) {
    braille_engine_reset();
    output_count = notice_count = noticed_count = 0;
}

static struct zmk_behavior_binding_event chord(uint16_t mask) {
    size_t before = output_count, notices_before = notice_count;
    struct zmk_behavior_binding_event event = {
        .layer = 2, .timestamp = ++timestamp,
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
        .source = 1,
#endif
    };
    assert(mask <= BRAILLE_CHORD_MASK);
    if (!mask) {
        assert(braille_chord_resolved(0, event) == 0); /* no physical chord */
        return event;
    }
    for (unsigned bit = 0; bit < 9; ++bit) {
        if (mask & (1U << bit)) {
            event.position = bit;
            assert(braille_chord_key_event(1U << bit, true, event) == 0);
            assert(output_count == before && notice_count == notices_before);
        }
    }
    for (unsigned bit = 0; bit < 9; ++bit) {
        if (mask & (1U << bit)) {
            event.position = bit;
            assert(braille_chord_key_event(1U << bit, false, event) == 0);
            if (mask >> (bit + 1)) {
                assert(output_count == before && notice_count == notices_before);
            }
        }
    }
    return event;
}

static void expect_event(struct zmk_behavior_binding_event a,
                         struct zmk_behavior_binding_event b) {
    assert(a.layer == b.layer && a.position == b.position && a.timestamp == b.timestamp);
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
    assert(a.source == b.source);
#endif
}

static void expect_pair(size_t offset, uint32_t key, struct zmk_behavior_binding_event event) {
    assert(output_count >= offset + 2);
    for (size_t i = 0; i < 2; ++i) {
        const struct invocation *out = &output[offset + i];
        assert(strcmp(out->binding.behavior_dev, "kp") == 0);
        assert(out->binding.param1 == key && out->binding.param2 == 0);
        assert(out->pressed == (i == 0));
        expect_event(out->event, event);
    }
}

static const struct braille_rule *find_character(uint16_t mask) {
    const struct braille_rule_table *table = &braille_rules_computer8_characters;
    const struct braille_rule *found = NULL;
    for (size_t i = 0; i < table->count; ++i) {
        if (table->rules[i].chords[0] == mask) {
            assert(!found);
            found = &table->rules[i];
        }
    }
    return found;
}

static int ascii_for_mask(uint16_t mask) {
    for (size_t i = 0; i < COUNT(ascii_masks); ++i) {
        if (ascii_masks[i] == mask) { return (int)i + '!'; }
    }
    return 0;
}

static void test_rule_data(void) {
    const struct braille_mode *mode = braille_tables_find("computer8");
    assert(mode->characters->count == 94);
    assert(mode->commands == &braille_rules_computer8_commands);
    assert(braille_engine_validate_factory(mode->characters, mode->commands) == 0);
    assert(braille_tables_select("computer8") == 0);
    for (size_t i = 0; i < COUNT(ascii_masks); ++i) {
        const struct braille_rule *rule = find_character(ascii_masks[i]);
        assert(rule && rule->chord_count == 1 && rule->action_count == 1);
        assert(strcmp(rule->actions[0].behavior_dev, "kp") == 0);
        assert(rule->actions[0].param1 == host_key((unsigned char)(i + '!')));
        assert(rule->actions[0].param2 == 0);
        assert(rule->word_position == BRAILLE_WORD_POSITION_ANY);
        assert(rule->class == BRAILLE_CLASS_NEUTRAL && rule->sets == BRAILLE_CAPS_NONE);
        assert(rule->when.numeric == BRAILLE_CONDITION_ANY);
        assert(rule->when.caps == BRAILLE_CONDITION_ANY);
        assert(rule->when.word_start == BRAILLE_CONDITION_ANY);
        for (size_t j = 1; j < BRAILLE_RULE_MAX_CHORDS; ++j) { assert(!rule->chords[j]); }
        for (unsigned numeric = 0; numeric < 2; ++numeric) {
            for (int caps = BRAILLE_CAPS_NONE; caps <= BRAILLE_CAPS_PASSAGE; ++caps) {
                for (unsigned word_start = 0; word_start < 2; ++word_start) {
                    struct braille_context context = {numeric, caps, word_start};
                    assert(braille_rule_when_holds(&rule->when, &context));
                }
            }
        }
    }
    puts("ok: 94 unconditional neutral one-cell rules, correct HID usages and no factory collisions");
}

static void test_all_256_patterns(void) {
    size_t mapped = 0, reserved = 0, commands = 0;
    for (uint16_t mask = 0; mask < 256; ++mask) {
        reset();
        struct zmk_behavior_binding_event event = chord(mask);
        assert(braille_engine_pending_count() == 0);
        int ascii = ascii_for_mask(mask);
        if (ascii) {
            ++mapped;
            assert(output_count == 2 && !notice_count);
            expect_pair(0, host_key((unsigned char)ascii), event);
        } else if (mask == 0x40 || mask == 0x80) {
            ++commands;
            assert(!find_character(mask));
            assert(output_count == 2 && !notice_count);
            expect_pair(0, KEY(mask == 0x40 ? 0x2a : 0x28), event);
        } else {
            ++reserved;
            assert(!find_character(mask) && !output_count);
            assert(notice_count == (mask != 0));
            if (mask) {
                assert(noticed_count == 1 && noticed_cells[0] == mask);
                expect_event(noticed_event, event);
                /* No stale reserved cell consumes the following character. */
                event = chord(0x01);
                assert(output_count == 2 && notice_count == 1);
                expect_pair(0, host_key('a'), event);
            }
        }
    }
    assert(mapped == 94 && commands == 2 && reserved == 160);
    reset();
    struct zmk_behavior_binding_event event = chord(0x100);
    assert(output_count == 2 && !notice_count && !braille_engine_pending_count());
    expect_pair(0, host_key(' '), event);
    puts("ok: all 256 masks + Braille space; 159 enterable reserved cells each notify once and recover");
}

static void test_reserved_cells_can_be_overridden(void) {
    struct braille_override_layer overrides = braille_tables_overrides();
    overrides.rules[0] = (struct braille_rule){
        .chord_count = 1, .action_count = 1, .class = BRAILLE_CLASS_NEUTRAL,
        .actions = {{.behavior_dev = "kp", .param1 = SHIFT(0x35)}},
    };
    *overrides.count = 1;
    size_t tested = 0;
    for (uint16_t mask = 1; mask < 256; ++mask) {
        if (ascii_for_mask(mask) || mask == 0x40 || mask == 0x80) { continue; }
        overrides.rules[0].chords[0] = mask;
        reset();
        struct zmk_behavior_binding_event event = chord(mask);
        assert(output_count == 2 && !notice_count && !braille_engine_pending_count());
        expect_pair(0, host_key('~'), event);
        ++tested;
    }
    assert(tested == 159);
    *overrides.count = 0;
    puts("ok: every enterable reserved pattern is available to a live user override");
}

static void test_all_ascii_in_one_stream(void) {
    reset();
    for (size_t i = 0; i < COUNT(ascii_masks); ++i) {
        struct zmk_behavior_binding_event event = chord(ascii_masks[i]);
        assert(output_count == 2 * (i + 1) && !notice_count && !braille_engine_pending_count());
        expect_pair(2 * i, host_key((unsigned char)(i + '!')), event);
    }
    puts("ok: all 94 ASCII cells commit immediately in one stream with no UEB buffering");
}

int main(int argc, char **argv) {
    if (argc > 1) {
        /* Python's separate published-map and YAML oracles use this same path. */
        assert(strcmp(argv[1], "--sequence") == 0);
        assert(braille_tables_select("computer8") == 0);
        for (int i = 2; i < argc; ++i) {
            char *end;
            unsigned long mask = strtoul(argv[i], &end, 0);
            assert(*end == '\0' && mask <= BRAILLE_CHORD_MASK);
            chord((uint16_t)mask);
        }
        assert(!braille_engine_pending_count() && output_count % 2 == 0);
        for (size_t i = 0; i < output_count; i += 2) {
            expect_pair(i, output[i].binding.param1, output[i].event);
            printf("key %08x\n", (unsigned)output[i].binding.param1);
        }
        printf("notices %zu\n", notice_count);
        return 0;
    }
    test_rule_data();
    test_all_256_patterns();
    test_reserved_cells_can_be_overridden();
    test_all_ascii_in_one_stream();
    puts("all computer8 character tests passed");
    return 0;
}

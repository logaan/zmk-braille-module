// SPDX-License-Identifier: MIT
/* Tasks 9.3.2.1.3/9.3.2.2: real UEB1 + Computer8 tables and their own
 * commands, through selection/capture/matching/dispatch. In-memory override
 * fixtures exercise the common matcher, not a second translation path. */
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <braille_chord.h>
#include <braille_engine.h>
#include <braille_tables.h>

#define KEY(id) (0x070000UL | (id))
#define SHIFT(id) (0x02070000UL | (id))
#define COUNT(a) (sizeof(a) / sizeof((a)[0]))

static struct { const char *behavior; uint32_t key; bool pressed; } output[16];
static size_t output_count, notice_count;
static uint16_t dropped;

int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event, bool pressed) {
    (void)event;
    assert(output_count < COUNT(output) && binding->param2 == 0);
    output[output_count].behavior = binding->behavior_dev;
    output[output_count].key = binding->param1;
    output[output_count++].pressed = pressed;
    return 0;
}

void braille_engine_notify_dropped_prefix(const uint16_t *cells, uint8_t count,
                                          struct zmk_behavior_binding_event event) {
    (void)event;
    assert(count == 1);
    dropped = cells[0];
    ++notice_count;
}

static void clear_output(void) { output_count = notice_count = 0; }

static void chord(uint16_t mask) {
    struct zmk_behavior_binding_event event = {.layer = 2, .timestamp = 100};
    size_t before = output_count, notices_before = notice_count;
    for (unsigned state = 0; state < 2; ++state) {
        for (unsigned bit = 0; bit < 9; ++bit) {
            if (mask & (1U << bit)) {
                event.position = bit;
                assert(braille_chord_key_event(1U << bit, state == 0, event) == 0);
                if (state == 0 || mask >> (bit + 1)) {
                    assert(output_count == before && notice_count == notices_before);
                }
            }
        }
    }
}

static void pair(size_t offset, const char *behavior, uint32_t key) {
    assert(output_count >= offset + 2);
    for (size_t i = 0; i < 2; ++i) {
        assert(strcmp(output[offset + i].behavior, behavior) == 0);
        assert(output[offset + i].key == key && output[offset + i].pressed == (i == 0));
    }
}

static void expect_one(uint32_t key) {
    assert(output_count == 2 && !notice_count && !braille_engine_pending_count());
    pair(0, "kp", key);
}

static void expect_context(struct braille_context expected) {
    const struct braille_context *actual = braille_engine_context();
    assert(actual->numeric == expected.numeric && actual->caps == expected.caps &&
           actual->word_start == expected.word_start);
}

static void seed_context(struct braille_context context) {
    /* Test-only injection into the engine's non-const state through its const
     * inspection view. There are no production UEB transitions yet (phase 04),
     * so explicitly seed every reachable field combination without adding a
     * firmware-only-for-tests API or claiming contextual UEB is implemented. */
    *(struct braille_context *)braille_engine_context() = context;
}

static void test_context_isolation(void) {
    assert(braille_tables_select("computer8") == 0);
    const struct braille_rule_table *table = &braille_rules_computer8_characters;
    /* Compare each already independently verified real row in all 12 context
     * combinations: no UEB predicate, word position, transition or buffering. */
    for (unsigned numeric = 0; numeric < 2; ++numeric) {
        for (int caps = BRAILLE_CAPS_NONE; caps <= BRAILLE_CAPS_PASSAGE; ++caps) {
            for (unsigned word_start = 0; word_start < 2; ++word_start) {
                struct braille_context context = {numeric, caps, word_start};
                seed_context(context);
                for (size_t r = 0; r < table->count; ++r) {
                    const struct braille_rule *rule = &table->rules[r];
                    clear_output();
                    chord(rule->chords[0]);
                    expect_one(rule->actions[0].param1);
                    expect_context(context); /* neutral means no UEB transition */
                }
            }
        }
    }
    /* Former UEB prefixes are immediate literal characters, never indicators
     * or contractions, at a word start and after another character alike. */
    static const struct { uint16_t mask; uint32_t key; } literals[] = {
        {0x20, KEY(0x36)},   /* dot 6: comma, not capitals */
        {0x3c, SHIFT(0x20)}, /* dots 3456: #, not numeric mode */
        {0x30, KEY(0x33)},   /* dots 56: ;, not grade-1 mode */
        {0x26, KEY(0x25)},   /* dots 236: 8, not quotes/question mark */
        {0x21, SHIFT(0x25)}, /* dots 16: *, not ch */
        {0x08, KEY(0x35)},   /* dot 4: `, not a multi-cell prefix */
    };
    for (size_t i = 0; i < COUNT(literals); ++i) {
        clear_output();
        chord(literals[i].mask);
        expect_one(literals[i].key);
        clear_output();
        chord(0x01);
        expect_one(KEY(0x04)); /* always lowercase a, never capital or digit */
    }
    puts("ok: all 94 real Computer8 cells are immediate and neutral in every UEB context");
}

static void test_switch_discards_real_ueb_prefix(void) {
    assert(braille_tables_select("ueb1") == 0);
    clear_output();
    chord(0x08); /* real strict prefix for @ etc. */
    assert(!output_count && !notice_count && braille_engine_pending_count() == 1);
    seed_context((struct braille_context){true, BRAILLE_CAPS_PASSAGE, false});
    assert(braille_tables_select("computer8") == 0);
    assert(!output_count && !notice_count && !braille_engine_pending_count());
    expect_context(BRAILLE_CONTEXT_INIT);
    chord(0x01);
    expect_one(KEY(0x04)); /* a, not the stale UEB prefix's @ */

    /* Same cell, different real table: 2 is digit 1 in Computer8, comma in UEB1. */
    clear_output();
    chord(0x02);
    expect_one(KEY(0x1e));
    seed_context((struct braille_context){true, BRAILLE_CAPS_WORD, false});
    assert(braille_tables_select("ueb1") == 0);
    expect_context(BRAILLE_CONTEXT_INIT);
    clear_output();
    chord(0x02);
    expect_one(KEY(0x36));
    clear_output();
    chord(0x08);
    assert(!output_count && braille_engine_pending_count() == 1);
    chord(0x01);
    expect_one(SHIFT(0x1f)); /* the UEB1 sequence still yields @ */
    puts("ok: switching resets seeded context and silently discards real UEB pending cells");
}

static struct braille_rule override(uint16_t first, uint16_t second, uint32_t key) {
    return (struct braille_rule){
        .chords = {first, second}, .chord_count = second ? 2 : 1,
        .actions = {{.behavior_dev = "kp", .param1 = key}}, .action_count = 1,
        .class = BRAILLE_CLASS_NEUTRAL,
    };
}

static void test_shared_overrides_and_commands(void) {
    static const char *const names[] = {"ueb1", "computer8"};
    struct braille_override_layer overrides = braille_tables_overrides();
    assert(overrides.capacity >= 4);
    overrides.rules[0] = override(0xc0, 0, SHIFT(0x1b)); /* reserved -> X */
    overrides.rules[1] = override(0x01, 0, SHIFT(0x1d)); /* mapped a -> Z */
    overrides.rules[2] = override(0xc1, 0xc2, SHIFT(0x38)); /* reserved sequence -> ? */
    overrides.rules[3] = override(0x03, 0x03, SHIFT(0x1c)); /* b b -> Y */
    *overrides.count = 4;
    for (size_t m = 0; m < COUNT(names); ++m) {
        const struct braille_mode *mode = braille_tables_find(names[m]);
        assert(braille_tables_select(names[m]) == 0);
        assert(braille_engine_validate_factory(mode->characters, mode->commands) == 0);
        clear_output();
        chord(0xc0);
        expect_one(SHIFT(0x1b));
        clear_output();
        chord(0x01);
        expect_one(SHIFT(0x1d));
        clear_output();
        chord(0xc1);
        assert(!output_count && !notice_count && braille_engine_pending_count() == 1);
        chord(0xc2);
        expect_one(SHIFT(0x38));
        clear_output();
        chord(0xc1);
        chord(0x01); /* orphan prefix drops; live single override still wins */
        assert(output_count == 2 && notice_count == 1 && dropped == 0xc1);
        assert(!braille_engine_pending_count());
        pair(0, "kp", SHIFT(0x1d));
        clear_output();
        chord(0x03);
        assert(!output_count && !notice_count && braille_engine_pending_count() == 1);
        chord(0x03);
        expect_one(SHIFT(0x1c));
        clear_output();
        chord(0x03);
        chord(0x05); /* longer override ruled out: b then k, exactly once */
        assert(output_count == 4 && !notice_count && !braille_engine_pending_count());
        pair(0, "kp", KEY(0x05));
        pair(2, "kp", KEY(0x0e));
        for (size_t c = 0; c < mode->commands->count; ++c) {
            const struct braille_rule *command = &mode->commands->rules[c];
            clear_output();
            chord(command->chords[0]);
            assert(output_count == 2 && !notice_count && !braille_engine_pending_count());
            pair(0, command->actions[0].behavior_dev, command->actions[0].param1);
        }
        /* Every whitespace command preserves the pending complete b before
         * acting, including the dot-8 Return aliases. Navigation discards it. */
        static const uint16_t whitespace[] = {0x100, 0x11e, 0x111, 0x80, 0x180};
        for (size_t c = 0; c < COUNT(whitespace); ++c) {
            clear_output();
            chord(0x03);
            chord(whitespace[c]);
            assert(output_count == 4 && !notice_count && !braille_engine_pending_count());
            pair(0, "kp", KEY(0x05));
            pair(2, "kp", KEY(c == 0 ? 0x2c : (c == 1 ? 0x2b : 0x28)));
        }
        clear_output();
        chord(0x03);
        chord(0x101);
        assert(output_count == 2 && notice_count == 1 && dropped == 0x03);
        pair(0, "kp", KEY(0x50));
    }
    *overrides.count = 0;
    puts("ok: both modes retain live overrides, multi-cell matching, every command and whitespace path");
}

static void test_switch_discards_override_prefix_in_both_directions(void) {
    static const char *const names[] = {"ueb1", "computer8"};
    struct braille_override_layer overrides = braille_tables_overrides();
    overrides.rules[0] = override(0x08, 0x03, SHIFT(0x1b)); /* prefix is real Computer8 ` */
    *overrides.count = 1;
    for (size_t m = 0; m < COUNT(names); ++m) {
        assert(braille_tables_select(names[m]) == 0);
        clear_output();
        chord(0x08);
        assert(!output_count && !notice_count && braille_engine_pending_count() == 1);
        seed_context((struct braille_context){true, BRAILLE_CAPS_PASSAGE, false});
        assert(braille_tables_select("missing-mode") == -ENOENT);
        assert(braille_engine_pending_count() == 1); /* failed selection changes nothing */
        expect_context((struct braille_context){true, BRAILLE_CAPS_PASSAGE, false});
        assert(braille_tables_select(names[1 - m]) == 0);
        assert(!output_count && !notice_count && !braille_engine_pending_count());
        expect_context(BRAILLE_CONTEXT_INIT);
        chord(0x03);
        expect_one(KEY(0x05)); /* fresh b, not completion of stale override */
        clear_output();
        chord(0x08);
        assert(!output_count && braille_engine_pending_count() == 1);
        chord(0x03);
        expect_one(SHIFT(0x1b)); /* shared override itself survives selection */
    }
    *overrides.count = 0;
    /* Removing the explicit multi-cell override restores immediate backtick. */
    assert(braille_tables_select("computer8") == 0);
    clear_output();
    chord(0x08);
    expect_one(KEY(0x35));
    puts("ok: both switch directions discard pending overrides, reset context and retain shared rules");
}

int main(void) {
    test_context_isolation();
    test_switch_discards_real_ueb_prefix();
    test_shared_overrides_and_commands();
    test_switch_discards_override_prefix_in_both_directions();
    puts("all computer8 context-isolation/mode-switch tests passed");
    return 0;
}

// SPDX-License-Identifier: MIT
/* Real capture -> mode registry -> common matcher -> ordered behavior dispatch.
 * HID expectations are independent of ZMK's keycode macros. --sequence emits
 * machine-readable observations for the rulebook/YAML conformance suite. */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <braille_chord.h>
#include <braille_engine.h>
#include <braille_settings.h>
#include <braille_tables.h>

#define COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define KEY(id) (0x070000UL | (id))
#define SHIFT(id) (0x02070000UL | (id))
static uint32_t keys[512];
static size_t key_count;
static uint16_t dropped[64][BRAILLE_RULE_MAX_CHORDS];
static uint8_t drop_lengths[64];
static size_t drop_count;
static bool awaiting_release;
static struct zmk_behavior_binding_event press_event;
static int64_t timestamp;

int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event, bool pressed) {
    assert(strcmp(binding->behavior_dev, "kp") == 0 && binding->param2 == 0);
    assert(pressed != awaiting_release);
    if (pressed) {
        assert(key_count < COUNT(keys));
        keys[key_count++] = binding->param1;
        press_event = event;
    } else {
        assert(keys[key_count - 1] == binding->param1);
        assert(event.position == press_event.position && event.timestamp == press_event.timestamp &&
               event.layer == press_event.layer);
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
        assert(event.source == press_event.source);
#endif
    }
    awaiting_release = pressed;
    return 0;
}
void braille_engine_notify_dropped_prefix(const uint16_t *cells, uint8_t count,
                                          struct zmk_behavior_binding_event event) {
    (void)event;
    assert(drop_count < COUNT(dropped) && count && count <= BRAILLE_RULE_MAX_CHORDS);
    memcpy(dropped[drop_count], cells, count * sizeof(*cells));
    drop_lengths[drop_count++] = count;
}
static void reset(void) {
    braille_engine_reset();
    key_count = drop_count = 0;
    assert(!awaiting_release);
}
static void chord(uint16_t mask) {
    assert(mask && !(mask & ~BRAILLE_CHORD_MASK));
    size_t before = key_count;
    struct zmk_behavior_binding_event event = {.layer = 0, .timestamp = ++timestamp,
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
        .source = 1,
#endif
    };
    for (unsigned state = 0; state < 2; ++state) {
        for (unsigned bit = 0; bit < 9; ++bit) {
            if (mask & (1U << bit)) {
                event.position = bit;
                assert(braille_chord_key_event(1U << bit, state == 0, event) == 0);
                if (!state || mask >> (bit + 1)) assert(key_count == before);
            }
        }
    }
    assert(!awaiting_release);
}
static void sequence(const uint16_t *cells, size_t count) {
    for (size_t i = 0; i < count; ++i) chord(cells[i]);
}
static void expect_keys(const uint32_t *expected, size_t count) {
    assert(key_count == count && memcmp(keys, expected, count * sizeof(*keys)) == 0);
    assert(!braille_engine_pending_count() && !drop_count);
}

static void test_numbers(void) {
    static const uint16_t letters[] = {0x01, 0x03, 0x09, 0x19, 0x11, 0x0b, 0x1b, 0x13, 0x0a, 0x1a};
    for (size_t i = 0; i < COUNT(letters); ++i) {
        reset();
        chord(0x3c);
        assert(!key_count && braille_engine_pending_count() == 1 && !braille_engine_context()->numeric);
        chord(letters[i]);
        uint32_t digit = KEY(0x1e + i);
        expect_keys(&digit, 1);
        assert(braille_engine_context()->numeric);
        chord(letters[i]);
        assert(key_count == 2 && keys[1] == digit && braille_engine_context()->numeric);
        chord(0x10); /* numeric space prefix stays silent until its following digit */
        assert(key_count == 2 && braille_engine_pending_count() == 1);
        chord(letters[i]);
        assert(key_count == 4 && keys[2] == KEY(0x2c) && keys[3] == digit);
        assert(braille_engine_context()->numeric && !braille_engine_pending_count());
    }
    static const uint16_t decimal[] = {0x02, 0x32};
    for (size_t i = 0; i < COUNT(decimal); ++i) {
        reset(); chord(0x3c); chord(decimal[i]); chord(0x01); chord(decimal[i]); chord(0x03);
        uint32_t punct = KEY(i ? 0x37 : 0x36);
        uint32_t expected[] = {punct, KEY(0x1e), punct, KEY(0x1f)};
        expect_keys(expected, COUNT(expected));
        assert(braille_engine_context()->numeric);
    }
    static const uint16_t endings[] = {0x100, 0x05, 0x24, 0x12, 0x16, 0x04, 0x11e, 0x111, 0x80, 0x180};
    for (size_t i = 0; i < COUNT(endings); ++i) {
        reset(); chord(0x3c); chord(0x01); chord(endings[i]);
        assert(!braille_engine_context()->numeric);
        chord(0x03);
        assert(keys[key_count - 1] == KEY(0x05)); /* b, never stale digit 2 */
    }
    reset();
    static const uint16_t parentheses[] = {0x3c, 0x1b, 0x10, 0x23, 0x03, 0x10, 0x1c};
    sequence(parentheses, COUNT(parentheses));
    static const uint32_t paren_expected[] = {KEY(0x24), SHIFT(0x26), KEY(0x05), SHIFT(0x27)};
    expect_keys(paren_expected, COUNT(paren_expected));
    reset(); chord(0x3c); chord(0x24); /* invalid numeric prefix is dropped, hyphen re-read */
    assert(key_count == 1 && keys[0] == KEY(0x2d) && drop_count == 1 && dropped[0][0] == 0x3c);
    assert(!braille_engine_context()->numeric && !braille_engine_pending_count());
    puts("ok: twelve numeric indicators, runs, punctuation/spaces, termination and orphan prefix");
}

static void print_observation(void) {
    printf("{\"keys\":[");
    for (size_t i = 0; i < key_count; ++i) printf("%s%u", i ? "," : "", (unsigned)keys[i]);
    printf("],\"pending\":[");
    for (uint8_t i = 0; i < braille_engine_pending_count(); ++i)
        printf("%s%u", i ? "," : "", braille_engine_pending_cells()[i]);
    printf("],\"drops\":[");
    for (size_t i = 0; i < drop_count; ++i) {
        printf("%s[", i ? "," : "");
        for (uint8_t c = 0; c < drop_lengths[i]; ++c)
            printf("%s%u", c ? "," : "", dropped[i][c]);
        printf("]");
    }
    const struct braille_context *ctx = braille_engine_context();
    printf("],\"context\":[%u,%u,%u]}\n", ctx->numeric, ctx->caps, ctx->word_start);
}
int main(int argc, char **argv) {
    assert(braille_tables_select("ueb1") == 0);
    assert(braille_engine_validate_factory(&braille_rules_ueb1_characters,
                                         &braille_rules_ueb1_commands) == 0);
    if (argc > 1) {
        assert(strcmp(argv[1], "--sequence") == 0);
        for (int i = 2; i < argc; ++i) {
            char *end;
            unsigned long cell = strtoul(argv[i], &end, 0);
            assert(!*end && cell && cell <= BRAILLE_CHORD_MASK);
            chord(cell);
        }
        print_observation();
        return 0;
    }
    test_numbers();
    puts("all contextual UEB1 unit tests passed");
    return 0;
}

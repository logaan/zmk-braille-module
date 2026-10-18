// SPDX-License-Identifier: MIT
/*
 * Host tests for the 26 six-dot alphabet cells (src/braille_rules_grade1.c),
 * exercised through the real shared path: braille_tables selection, the
 * braille_engine matcher, and braille_dispatch's ordered action invocation,
 * with `zmk_behavior_invoke_binding` as a recording stub. Every letter must
 * produce one ordinary lowercase host keystroke (`&kp` of the letter usage,
 * no modifiers), never a Unicode Braille cell. Compiled and run natively by
 * test_alphabet.py, with and without CONFIG_ZMK_SPLIT.
 */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <braille_chord.h>
#include <braille_dispatch.h>
#include <braille_engine.h>
#include <braille_rules.h>
#include <braille_tables.h>
#include <zmk/behavior.h>

/* --- Invocation log -------------------------------------------------------- */

struct invoke_entry {
    const char *dev;
    uint32_t param1;
    uint32_t param2;
    bool pressed;
    struct zmk_behavior_binding_event event;
};

static struct invoke_entry log_entries[64];
static size_t log_count;

static void log_reset(void) { log_count = 0; }

/* Recording stub; the real dispatcher is ZMK's behavior runtime. */
int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event, bool pressed) {
    assert(log_count < sizeof(log_entries) / sizeof(log_entries[0]));
    log_entries[log_count] = (struct invoke_entry){
        .dev = binding->behavior_dev,
        .param1 = binding->param1,
        .param2 = binding->param2,
        .pressed = pressed,
        .event = event,
    };
    log_count++;
    return 0;
}

/* --- The ICEB six-dot alphabet (rulebook section 2.5) ----------------------- */

/* ZMK_HID_USAGE(HID_USAGE_KEY, id) = (0x07 << 16) | id. */
#define KEY_USAGE(id) ((0x07UL << 16) | (id))

struct alphabet_entry {
    char letter;
    uint16_t chord; /* dot n is bit n-1 */
    uint8_t usage;  /* HID keyboard usage id, a..z = 0x04..0x1D */
};

static const struct alphabet_entry alphabet[26] = {
    {'a', 0x01, 0x04}, /* dots 1             */
    {'b', 0x03, 0x05}, /* dots 1, 2          */
    {'c', 0x09, 0x06}, /* dots 1, 4          */
    {'d', 0x19, 0x07}, /* dots 1, 4, 5       */
    {'e', 0x11, 0x08}, /* dots 1, 5          */
    {'f', 0x0B, 0x09}, /* dots 1, 2, 4       */
    {'g', 0x1B, 0x0A}, /* dots 1, 2, 4, 5    */
    {'h', 0x13, 0x0B}, /* dots 1, 2, 5       */
    {'i', 0x0A, 0x0C}, /* dots 2, 4          */
    {'j', 0x1A, 0x0D}, /* dots 2, 4, 5       */
    {'k', 0x05, 0x0E}, /* dots 1, 3          */
    {'l', 0x07, 0x0F}, /* dots 1, 2, 3       */
    {'m', 0x0D, 0x10}, /* dots 1, 3, 4       */
    {'n', 0x1D, 0x11}, /* dots 1, 3, 4, 5    */
    {'o', 0x15, 0x12}, /* dots 1, 3, 5       */
    {'p', 0x0F, 0x13}, /* dots 1, 2, 3, 4    */
    {'q', 0x1F, 0x14}, /* dots 1, 2, 3, 4, 5 */
    {'r', 0x17, 0x15}, /* dots 1, 2, 3, 5    */
    {'s', 0x0E, 0x16}, /* dots 2, 3, 4       */
    {'t', 0x1E, 0x17}, /* dots 2, 3, 4, 5    */
    {'u', 0x25, 0x18}, /* dots 1, 3, 6       */
    {'v', 0x27, 0x19}, /* dots 1, 2, 3, 6    */
    {'w', 0x3A, 0x1A}, /* dots 2, 4, 5, 6    */
    {'x', 0x2D, 0x1B}, /* dots 1, 3, 4, 6    */
    {'y', 0x3D, 0x1C}, /* dots 1, 3, 4, 5, 6 */
    {'z', 0x35, 0x1D}, /* dots 1, 3, 5, 6    */
};

static const struct zmk_behavior_binding_event ev = {
    .layer = 0,
    .position = 5,
    .timestamp = 42,
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
    .source = 1,
#endif
};

static void chord(uint16_t mask) {
    int ret = braille_chord_resolved(mask, ev);
    assert(ret == 0);
}

/* --- Tests ------------------------------------------------------------------ */

/* 1. Table shape: exactly 26 single-chord, single-action letter rules in the
 *    one common representation: an ordinary `&kp` binding, any context, the
 *    letter class, no capitals side effect. */
static void test_table_shape(void) {
    const struct braille_rule_table *t = &braille_rules_ueb1_characters;
    assert(t->count == 26);
    for (size_t i = 0; i < t->count; ++i) {
        const struct braille_rule *r = &t->rules[i];
        assert(r->chord_count == 1);
        assert(r->chords[0] == alphabet[i].chord);
        assert((r->chords[0] & BRAILLE_CHORD_SPACE_BIT) == 0); /* dot-only */
        assert(r->action_count == 1);
        assert(strcmp(r->actions[0].behavior_dev, DEVICE_DT_NAME(DT_NODELABEL(kp))) == 0);
        assert(r->actions[0].param1 == KEY_USAGE(alphabet[i].usage));
        assert(r->actions[0].param2 == 0);
        assert(r->word_position == BRAILLE_WORD_POSITION_ANY);
        assert(r->when.numeric == BRAILLE_CONDITION_ANY);
        assert(r->when.caps == BRAILLE_CONDITION_ANY);
        assert(r->when.word_start == BRAILLE_CONDITION_ANY);
        assert(r->class == BRAILLE_CLASS_LETTER);
        assert(r->sets == BRAILLE_CAPS_NONE);
    }
    printf("ok: table shape\n");
}

/* 2. Selection: the ueb1 mode validates (no collision between the alphabet
 *    and the mode's fixture command table) and the devicetree default names
 *    it. */
static void test_selection(void) {
    assert(braille_engine_validate_factory(&braille_rules_ueb1_characters,
                                           &braille_rules_ueb1_commands) == 0);
    assert(braille_tables_select("ueb1") == 0);
    assert(braille_tables_active() != NULL);
    assert(strcmp(braille_tables_active(), "ueb1") == 0);
    assert(braille_tables_select_default() == 0);
    assert(strcmp(braille_tables_active(), "ueb1") == 0);
    printf("ok: selection\n");
}

/* 3. All 26 mappings through the shared capture/matching/dispatch path: each
 *    chord resolves to exactly one press/release pair of the letter usage on
 *    the keycode behavior, with the event forwarded unchanged. A lone letter
 *    usage with no modifier action is an ordinary lowercase host keystroke. */
static void test_all_26_mappings(void) {
    for (size_t i = 0; i < 26; ++i) {
        log_reset();
        chord(alphabet[i].chord);
        assert(log_count == 2);
        const struct invoke_entry *press = &log_entries[0];
        const struct invoke_entry *release = &log_entries[1];
        assert(strcmp(press->dev, "kp") == 0);
        assert(strcmp(release->dev, "kp") == 0);
        assert(press->param1 == KEY_USAGE(alphabet[i].usage));
        assert(release->param1 == KEY_USAGE(alphabet[i].usage));
        assert(press->param2 == 0 && release->param2 == 0);
        assert(press->pressed && !release->pressed);
        assert(memcmp(&press->event, &ev, sizeof(ev)) == 0);
        assert(memcmp(&release->event, &ev, sizeof(ev)) == 0);
        /* Ordinary lowercase letters: HID usages 0x04..0x1D in a..z order,
         * pressed with no sticky-key or other modifier action. */
        assert(alphabet[i].usage == 0x04 + (alphabet[i].letter - 'a'));
        assert(braille_engine_pending_count() == 0); /* nothing left pending */
    }
    printf("ok: all 26 mappings\n");
}

/* 4. The full alphabet typed consecutively yields 26 keystrokes in order. */
static void test_alphabet_sequence(void) {
    log_reset();
    for (size_t i = 0; i < 26; ++i) {
        chord(alphabet[i].chord);
    }
    assert(log_count == 52);
    for (size_t i = 0; i < 26; ++i) {
        assert(log_entries[2 * i].pressed);
        assert(!log_entries[2 * i + 1].pressed);
        assert(log_entries[2 * i].param1 == KEY_USAGE(alphabet[i].usage));
        assert(log_entries[2 * i + 1].param1 == KEY_USAGE(alphabet[i].usage));
    }
    assert(braille_engine_pending_count() == 0);
    printf("ok: alphabet sequence\n");
}

/* 5. Six-dot cells outside the alphabet produce no host output and leave
 *    nothing pending; the alphabet still works afterwards. (Literary signs
 *    for these cells arrive in phase 03.) */
static void test_unmapped_cells_silent(void) {
    static const uint16_t unmapped[] = {
        0x04, /* dot 3            */
        0x20, /* dot 6            */
        0x06, /* dots 2, 3        */
        0x12, /* dots 2, 5        */
        0x24, /* dots 3, 6        */
        0x3F, /* dots 1-6 (⠿)    */
    };
    for (size_t i = 0; i < sizeof(unmapped) / sizeof(unmapped[0]); ++i) {
        log_reset();
        chord(unmapped[i]);
        assert(log_count == 0);
        assert(braille_engine_pending_count() == 0);
    }
    log_reset();
    chord(alphabet[0].chord); /* a */
    assert(log_count == 2);
    printf("ok: unmapped cells silent\n");
}

int main(void) {
    test_table_shape();
    test_selection();
    test_all_26_mappings();
    test_alphabet_sequence();
    test_unmapped_cells_silent();
    printf("test_alphabet: all tests passed\n");
    return 0;
}

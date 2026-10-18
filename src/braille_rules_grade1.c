// SPDX-License-Identifier: MIT

#include <braille_rules.h>
#include <braille_tables.h>

/*
 * The `ueb1` character table: the 26 six-dot alphabet cells of the ICEB
 * rulebook (The Rules of Unified English Braille, 3rd edition 2024, section
 * 2.5), each producing one ordinary lowercase host keystroke through `&kp`.
 * Dot n is bit n-1 of the nine-bit chord mask (braille_chord.h); every cell
 * is a single dot-only chord, so it can never collide with the mode's
 * Braille-space command chords (bit 8). Phase 03 completes the Grade 1
 * literary signs in this same representation.
 *
 * A single `&kp` press/release with a letter usage and no modifiers yields
 * the lowercase letter on a US host layout; capitalization arrives with
 * phase 03's indicator rules, not with modifier actions here.
 *
 * ZMK_HID_USAGE(HID_USAGE_KEY, id) = (0x07 << 16) | id, as in
 * dt-bindings/zmk/keys.h; HID keyboard usage ids are dt-bindings/zmk/hid_usage.h
 * (a..z = 0x04..0x1D). The behavior device is named exactly as firmware
 * keymaps name it.
 */
#define BRAILLE_KEY_USAGE(id) ((0x07UL << 16) | (id))

#define BRAILLE_WHEN_ANY {BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY}

/* One six-dot cell -> one lowercase letter keystroke. */
#define BRAILLE_LETTER_RULE(chord, usage)                                                          \
    {                                                                                              \
        .chords = {chord},                                                                         \
        .chord_count = 1,                                                                          \
        .actions = {{.behavior_dev = DEVICE_DT_NAME(DT_NODELABEL(kp)),                             \
                     .param1 = BRAILLE_KEY_USAGE(usage)}},                                         \
        .action_count = 1,                                                                         \
        .when = BRAILLE_WHEN_ANY,                                                                  \
        .class = BRAILLE_CLASS_LETTER,                                                             \
    }

/* Dot bits: dot 1 = 0x01, dot 2 = 0x02, ..., dot 6 = 0x20 (dots 7/8 unused). */
static const struct braille_rule ueb1_alphabet_rules[] = {
    BRAILLE_LETTER_RULE(0x01, 0x04), /* a: dots 1             */
    BRAILLE_LETTER_RULE(0x03, 0x05), /* b: dots 1, 2          */
    BRAILLE_LETTER_RULE(0x09, 0x06), /* c: dots 1, 4          */
    BRAILLE_LETTER_RULE(0x19, 0x07), /* d: dots 1, 4, 5       */
    BRAILLE_LETTER_RULE(0x11, 0x08), /* e: dots 1, 5          */
    BRAILLE_LETTER_RULE(0x0B, 0x09), /* f: dots 1, 2, 4       */
    BRAILLE_LETTER_RULE(0x1B, 0x0A), /* g: dots 1, 2, 4, 5    */
    BRAILLE_LETTER_RULE(0x13, 0x0B), /* h: dots 1, 2, 5       */
    BRAILLE_LETTER_RULE(0x0A, 0x0C), /* i: dots 2, 4          */
    BRAILLE_LETTER_RULE(0x1A, 0x0D), /* j: dots 2, 4, 5       */
    BRAILLE_LETTER_RULE(0x05, 0x0E), /* k: dots 1, 3          */
    BRAILLE_LETTER_RULE(0x07, 0x0F), /* l: dots 1, 2, 3       */
    BRAILLE_LETTER_RULE(0x0D, 0x10), /* m: dots 1, 3, 4       */
    BRAILLE_LETTER_RULE(0x1D, 0x11), /* n: dots 1, 3, 4, 5    */
    BRAILLE_LETTER_RULE(0x15, 0x12), /* o: dots 1, 3, 5       */
    BRAILLE_LETTER_RULE(0x0F, 0x13), /* p: dots 1, 2, 3, 4    */
    BRAILLE_LETTER_RULE(0x1F, 0x14), /* q: dots 1, 2, 3, 4, 5 */
    BRAILLE_LETTER_RULE(0x17, 0x15), /* r: dots 1, 2, 3, 5    */
    BRAILLE_LETTER_RULE(0x0E, 0x16), /* s: dots 2, 3, 4       */
    BRAILLE_LETTER_RULE(0x1E, 0x17), /* t: dots 2, 3, 4, 5    */
    BRAILLE_LETTER_RULE(0x25, 0x18), /* u: dots 1, 3, 6       */
    BRAILLE_LETTER_RULE(0x27, 0x19), /* v: dots 1, 2, 3, 6    */
    BRAILLE_LETTER_RULE(0x3A, 0x1A), /* w: dots 2, 4, 5, 6    */
    BRAILLE_LETTER_RULE(0x2D, 0x1B), /* x: dots 1, 3, 4, 6    */
    BRAILLE_LETTER_RULE(0x3D, 0x1C), /* y: dots 1, 3, 4, 5, 6 */
    BRAILLE_LETTER_RULE(0x35, 0x1D), /* z: dots 1, 3, 5, 6    */
};

const struct braille_rule_table braille_rules_ueb1_characters = {
    .rules = ueb1_alphabet_rules,
    .count = sizeof(ueb1_alphabet_rules) / sizeof(ueb1_alphabet_rules[0]),
};

// SPDX-License-Identifier: MIT

#include <dt-bindings/zmk/keys.h>
#include <braille_rules.h>
#include <braille_tables.h>

/*
 * The `ueb1` character table: the 26 six-dot alphabet cells, seven
 * context-free core punctuation signs, and 21 specified two-cell symbols.
 * Authority: ICEB, The Rules of Unified English Braille, 3rd edition 2024,
 * sections 2.5, 3 and 7; per-symbol citations are recorded in
 * references/research/ueb-literary-signs.yaml.
 * Every row produces an ordinary US-layout host keystroke through `&kp`.
 * Dot n is bit n-1 of the nine-bit chord mask (braille_chord.h); these
 * six-dot cells collide with neither space chords nor dot-7/8 commands.
 *
 * Letter usages have no modifiers, so they yield lowercase on a US host
 * layout. Punctuation and symbols use ZMK keycodes with implicit Shift
 * where required. Phase 04 owns capitalization and context-dependent signs;
 * dots 2,3,6 (question mark/opening quotation mark) must stay unmapped here.
 *
 * ZMK_HID_USAGE(HID_USAGE_KEY, id) = (0x07 << 16) | id, as in
 * dt-bindings/zmk/keys.h; HID keyboard usage ids are dt-bindings/zmk/hid_usage.h
 * (a..z = 0x04..0x1D). The behavior device is named exactly as firmware
 * keymaps name it.
 */
#define BRAILLE_KEY_USAGE(id) ((0x07UL << 16) | (id))

#define BRAILLE_WHEN_ANY {BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY}

/* One or two ordered six-dot cells -> one host key, with class metadata. */
#define BRAILLE_CHARACTER_RULE(chord, key, cls)                                             \
    {                                                                                      \
        .chords = {chord}, .chord_count = 1,                                                \
        .actions = {{.behavior_dev = DEVICE_DT_NAME(DT_NODELABEL(kp)), .param1 = key}},       \
        .action_count = 1, .when = BRAILLE_WHEN_ANY, .class = cls,                            \
    }
#define BRAILLE_LETTER_RULE(chord, usage)                                                    \
    BRAILLE_CHARACTER_RULE(chord, BRAILLE_KEY_USAGE(usage), BRAILLE_CLASS_LETTER)
#define BRAILLE_SYMBOL_RULE(first, second, key, cls)                                        \
    {                                                                                      \
        .chords = {first, second}, .chord_count = 2,                                         \
        .actions = {{.behavior_dev = DEVICE_DT_NAME(DT_NODELABEL(kp)), .param1 = key}},       \
        .action_count = 1, .when = BRAILLE_WHEN_ANY, .class = cls,                            \
    }

/* Dot bits: dot 1 = 0x01, dot 2 = 0x02, ..., dot 6 = 0x20 (dots 7/8 unused). */
static const struct braille_rule ueb1_character_rules[] = {
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

    /* ICEB section 7, p.75. Comma/full stop remain numeric punctuation
     * (6.2.1); apostrophe is an opening symbol (2.6.2); hyphen bounds a
     * word (2.6.1). Classes are data only until phase-04 transitions. */
    BRAILLE_CHARACTER_RULE(0x02, COMMA, BRAILLE_CLASS_NUMERIC_PUNCTUATION), /* comma: 2      */
    BRAILLE_CHARACTER_RULE(0x06, SEMI, BRAILLE_CLASS_OTHER),               /* semicolon: 23 */
    BRAILLE_CHARACTER_RULE(0x12, COLON, BRAILLE_CLASS_OTHER),              /* colon: 25     */
    BRAILLE_CHARACTER_RULE(0x16, EXCL, BRAILLE_CLASS_OTHER),               /* exclamation: 235 */
    BRAILLE_CHARACTER_RULE(0x04, SQT, BRAILLE_CLASS_OPENING),              /* apostrophe: 3 */
    BRAILLE_CHARACTER_RULE(0x24, MINUS, BRAILLE_CLASS_HYPHEN),             /* hyphen: 36    */
    BRAILLE_CHARACTER_RULE(0x32, DOT, BRAILLE_CLASS_NUMERIC_PUNCTUATION),   /* full stop: 256 */

    /* Task 9.3.1.2, in its specified order. A first cell is a strict prefix,
     * not a host key or indicator action; the shared matcher holds it until
     * completion or drops it and re-reads a non-continuation from scratch.
     * Section 2.6.2 lists only ( [ { as opening brackets, not <. */
    BRAILLE_SYMBOL_RULE(0x08, 0x2F, AMPS, BRAILLE_CLASS_OTHER),  /* &: 4 then 12346; 3.1 */
    BRAILLE_SYMBOL_RULE(0x10, 0x14, STAR, BRAILLE_CLASS_OTHER),  /* *: 5 then 35; 3.3 */
    BRAILLE_SYMBOL_RULE(0x08, 0x01, AT, BRAILLE_CLASS_OTHER),    /* @: 4 then 1; 3.7 */
    BRAILLE_SYMBOL_RULE(0x08, 0x22, CARET, BRAILLE_CLASS_OTHER), /* ^: 4 then 26; 3.6 */
    BRAILLE_SYMBOL_RULE(0x38, 0x39, HASH, BRAILLE_CLASS_OTHER),  /* #: 456 then 1456; 3.19 */
    BRAILLE_SYMBOL_RULE(0x08, 0x0E, DLLR, BRAILLE_CLASS_OTHER),  /* $: 4 then 234; 3.10 */
    BRAILLE_SYMBOL_RULE(0x28, 0x34, PRCNT, BRAILLE_CLASS_OTHER), /* %: 46 then 356; 3.21 */
    BRAILLE_SYMBOL_RULE(0x08, 0x14, TILDE, BRAILLE_CLASS_OTHER), /* ~: 4 then 35; 3.25 */
    BRAILLE_SYMBOL_RULE(0x38, 0x0C, FSLH, BRAILLE_CLASS_OTHER),  /* /: 456 then 34; 7.4 */
    BRAILLE_SYMBOL_RULE(0x28, 0x24, UNDER, BRAILLE_CLASS_OTHER), /* _: 46 then 36; 7.2 */
    BRAILLE_SYMBOL_RULE(0x10, 0x16, PLUS, BRAILLE_CLASS_OTHER),  /* +: 5 then 235; 3.17 */
    BRAILLE_SYMBOL_RULE(0x10, 0x36, EQUAL, BRAILLE_CLASS_OTHER), /* =: 5 then 2356; 3.17 */
    BRAILLE_SYMBOL_RULE(0x08, 0x23, LT, BRAILLE_CLASS_OTHER),    /* <: 4 then 126; 3.17 */
    BRAILLE_SYMBOL_RULE(0x08, 0x1C, GT, BRAILLE_CLASS_OTHER),    /* >: 4 then 345; 3.17 */
    BRAILLE_SYMBOL_RULE(0x10, 0x23, LPAR, BRAILLE_CLASS_OPENING), /* (: 5 then 126; 7.1 */
    BRAILLE_SYMBOL_RULE(0x10, 0x1C, RPAR, BRAILLE_CLASS_OTHER),  /* ): 5 then 345; 7.1 */
    BRAILLE_SYMBOL_RULE(0x28, 0x23, LBKT, BRAILLE_CLASS_OPENING), /* [: 46 then 126; 7.1 */
    BRAILLE_SYMBOL_RULE(0x28, 0x1C, RBKT, BRAILLE_CLASS_OTHER),  /* ]: 46 then 345; 7.1 */
    BRAILLE_SYMBOL_RULE(0x38, 0x23, LBRC, BRAILLE_CLASS_OPENING), /* {: 456 then 126; 7.1 */
    BRAILLE_SYMBOL_RULE(0x38, 0x1C, RBRC, BRAILLE_CLASS_OTHER),  /* }: 456 then 345; 7.1 */
    BRAILLE_SYMBOL_RULE(0x38, 0x21, BSLH, BRAILLE_CLASS_OTHER),  /* backslash: 456 then 16; 7.1 */
};

const struct braille_rule_table braille_rules_ueb1_characters = {
    .rules = ueb1_character_rules,
    .count = sizeof(ueb1_character_rules) / sizeof(ueb1_character_rules[0]),
};

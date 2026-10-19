// SPDX-License-Identifier: MIT

#include <dt-bindings/zmk/keys.h>
#include <braille_rules.h>
#include <braille_tables.h>

/*
 * VoiceOver's Liblouis U.S. English (EBAE) 8-dot computer braille, restricted
 * to the 94 printable non-space ASCII characters. Source registration,
 * version-pinned definitions and all 256 mapped/reserved entries are in
 * references/research/computer8.yaml. This is NOT the legacy Duxbury US
 * variant: Liblouis uses dots 456 for underscore, not 4567.
 *
 * Each row is one cell -> one ordinary US-layout &kp press/release. Dot n
 * is bit n-1; dot 7 is part of the uppercase cell, not a prior indicator,
 * and adding dot 8 never aliases a printable character. All rows are
 * context-free and neutral; only explicit user sequence overrides can hold
 * an otherwise complete character pending in the shared matcher.
 *
 * Reserved patterns have NO rule, rather than a zero-action rule: absence
 * produces no output and reaches the existing dropped-prefix notice hook,
 * without claiming a factory mapping or colliding with overrides/commands.
 * Zero dots cannot form a chord. Braille space (0x100), dot 7 Delete (0x40),
 * and dot 8 Return (0x80) belong exclusively to the mode's command table.
 * Dots 78 together (0xc0) are reserved, not an upstream newline mapping.
 */
#define COMPUTER8_CHARACTER(chord, key)                                                     \
    {                                                                                      \
        .chords = {chord}, .chord_count = 1,                                                \
        .actions = {{.behavior_dev = DEVICE_DT_NAME(DT_NODELABEL(kp)), .param1 = key}},       \
        .action_count = 1, .class = BRAILLE_CLASS_NEUTRAL,                                   \
    }

/* ASCII order, 0x21 (!) through 0x7e (~). Omitted fields are ANY/NONE. */
static const struct braille_rule computer8_character_rules[] = {
    COMPUTER8_CHARACTER(0x2e, EXCL),  /* ! */
    COMPUTER8_CHARACTER(0x10, DQT),   /* " */
    COMPUTER8_CHARACTER(0x3c, HASH),  /* # */
    COMPUTER8_CHARACTER(0x2b, DLLR),  /* $ */
    COMPUTER8_CHARACTER(0x29, PRCNT), /* % */
    COMPUTER8_CHARACTER(0x2f, AMPS),  /* & */
    COMPUTER8_CHARACTER(0x04, SQT),   /* ' */
    COMPUTER8_CHARACTER(0x37, LPAR),  /* ( */
    COMPUTER8_CHARACTER(0x3e, RPAR),  /* ) */
    COMPUTER8_CHARACTER(0x21, STAR),  /* * */
    COMPUTER8_CHARACTER(0x2c, PLUS),  /* + */
    COMPUTER8_CHARACTER(0x20, COMMA), /* , */
    COMPUTER8_CHARACTER(0x24, MINUS), /* - */
    COMPUTER8_CHARACTER(0x28, DOT),   /* . */
    COMPUTER8_CHARACTER(0x0c, FSLH),  /* / */
    COMPUTER8_CHARACTER(0x34, N0),
    COMPUTER8_CHARACTER(0x02, N1),
    COMPUTER8_CHARACTER(0x06, N2),
    COMPUTER8_CHARACTER(0x12, N3),
    COMPUTER8_CHARACTER(0x32, N4),
    COMPUTER8_CHARACTER(0x22, N5),
    COMPUTER8_CHARACTER(0x16, N6),
    COMPUTER8_CHARACTER(0x36, N7),
    COMPUTER8_CHARACTER(0x26, N8),
    COMPUTER8_CHARACTER(0x14, N9),
    COMPUTER8_CHARACTER(0x31, COLON), /* : */
    COMPUTER8_CHARACTER(0x30, SEMI),  /* ; */
    COMPUTER8_CHARACTER(0x23, LT),    /* < */
    COMPUTER8_CHARACTER(0x3f, EQUAL), /* = */
    COMPUTER8_CHARACTER(0x1c, GT),    /* > */
    COMPUTER8_CHARACTER(0x39, QMARK), /* ? */
    COMPUTER8_CHARACTER(0x48, AT),    /* @ */
    COMPUTER8_CHARACTER(0x41, LS(A)),
    COMPUTER8_CHARACTER(0x43, LS(B)),
    COMPUTER8_CHARACTER(0x49, LS(C)),
    COMPUTER8_CHARACTER(0x59, LS(D)),
    COMPUTER8_CHARACTER(0x51, LS(E)),
    COMPUTER8_CHARACTER(0x4b, LS(F)),
    COMPUTER8_CHARACTER(0x5b, LS(G)),
    COMPUTER8_CHARACTER(0x53, LS(H)),
    COMPUTER8_CHARACTER(0x4a, LS(I)),
    COMPUTER8_CHARACTER(0x5a, LS(J)),
    COMPUTER8_CHARACTER(0x45, LS(K)),
    COMPUTER8_CHARACTER(0x47, LS(L)),
    COMPUTER8_CHARACTER(0x4d, LS(M)),
    COMPUTER8_CHARACTER(0x5d, LS(N)),
    COMPUTER8_CHARACTER(0x55, LS(O)),
    COMPUTER8_CHARACTER(0x4f, LS(P)),
    COMPUTER8_CHARACTER(0x5f, LS(Q)),
    COMPUTER8_CHARACTER(0x57, LS(R)),
    COMPUTER8_CHARACTER(0x4e, LS(S)),
    COMPUTER8_CHARACTER(0x5e, LS(T)),
    COMPUTER8_CHARACTER(0x65, LS(U)),
    COMPUTER8_CHARACTER(0x67, LS(V)),
    COMPUTER8_CHARACTER(0x7a, LS(W)),
    COMPUTER8_CHARACTER(0x6d, LS(X)),
    COMPUTER8_CHARACTER(0x7d, LS(Y)),
    COMPUTER8_CHARACTER(0x75, LS(Z)),
    COMPUTER8_CHARACTER(0x6a, LBKT),  /* [ */
    COMPUTER8_CHARACTER(0x73, BSLH),  /* backslash */
    COMPUTER8_CHARACTER(0x7b, RBKT),  /* ] */
    COMPUTER8_CHARACTER(0x58, CARET), /* ^ */
    COMPUTER8_CHARACTER(0x38, UNDER), /* _ */
    COMPUTER8_CHARACTER(0x08, GRAVE), /* ` */
    COMPUTER8_CHARACTER(0x01, A),
    COMPUTER8_CHARACTER(0x03, B),
    COMPUTER8_CHARACTER(0x09, C),
    COMPUTER8_CHARACTER(0x19, D),
    COMPUTER8_CHARACTER(0x11, E),
    COMPUTER8_CHARACTER(0x0b, F),
    COMPUTER8_CHARACTER(0x1b, G),
    COMPUTER8_CHARACTER(0x13, H),
    COMPUTER8_CHARACTER(0x0a, I),
    COMPUTER8_CHARACTER(0x1a, J),
    COMPUTER8_CHARACTER(0x05, K),
    COMPUTER8_CHARACTER(0x07, L),
    COMPUTER8_CHARACTER(0x0d, M),
    COMPUTER8_CHARACTER(0x1d, N),
    COMPUTER8_CHARACTER(0x15, O),
    COMPUTER8_CHARACTER(0x0f, P),
    COMPUTER8_CHARACTER(0x1f, Q),
    COMPUTER8_CHARACTER(0x17, R),
    COMPUTER8_CHARACTER(0x0e, S),
    COMPUTER8_CHARACTER(0x1e, T),
    COMPUTER8_CHARACTER(0x25, U),
    COMPUTER8_CHARACTER(0x27, V),
    COMPUTER8_CHARACTER(0x3a, W),
    COMPUTER8_CHARACTER(0x2d, X),
    COMPUTER8_CHARACTER(0x3d, Y),
    COMPUTER8_CHARACTER(0x35, Z),
    COMPUTER8_CHARACTER(0x2a, LBRC),  /* { */
    COMPUTER8_CHARACTER(0x33, PIPE),  /* | */
    COMPUTER8_CHARACTER(0x3b, RBRC),  /* } */
    COMPUTER8_CHARACTER(0x18, TILDE), /* ~ */
};

_Static_assert(sizeof(computer8_character_rules) / sizeof(computer8_character_rules[0]) == 94,
               "computer8: all 94 printable non-space ASCII characters");

const struct braille_rule_table braille_rules_computer8_characters = {
    .rules = computer8_character_rules,
    .count = sizeof(computer8_character_rules) / sizeof(computer8_character_rules[0]),
};

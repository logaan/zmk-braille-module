// SPDX-License-Identifier: MIT

#include <braille_engine.h>
#include <braille_rules.h>
#include <braille_tables.h>

/*
 * Fixture factory command tables, one per compiled mode, so navigation,
 * noncharacter functions, and the whitespace commit path are available in
 * every mode before phase 02 researches and implements the real per-mode
 * factory chords (macOS VoiceOver Braille keyboard conventions first, then
 * Android TalkBack, then Hable One; every real rule will carry a comment
 * naming its source).
 *
 * Every command chord carries the Braille-space bit (bit 8), so fixtures
 * can never collide with dot-only character cells; the Braille space alone
 * is the space sign. Key usages are ZMK HID usages (dt-bindings/zmk/keys.h:
 * the keyboard page 0x07 in the high half-word) and behavior devices are
 * named exactly as firmware keymaps name them.
 */

/* ZMK_HID_USAGE(HID_USAGE_KEY, id) = (0x07 << 16) | id. */
#define BRAILLE_KEY_USAGE(id) ((0x07UL << 16) | (id))

/* HID keyboard usage ids, dt-bindings/zmk/hid_usage.h. */
#define BRAILLE_KEY_RETURN 0x28
#define BRAILLE_KEY_TAB 0x2B
#define BRAILLE_KEY_SPACE 0x2C
#define BRAILLE_KEY_RIGHT_ARROW 0x4F
#define BRAILLE_KEY_LEFT_ARROW 0x50

/* Nine-bit fixture command chords; bit 8 is the Braille space
 * (braille_chord.h). All are fixtures pending phase-02 research. */
#define BRAILLE_CMD_SPACE 0x100          /* Braille space */
#define BRAILLE_CMD_RETURN (0x100 | 0x11) /* fixture: space + dots 1+5 */
#define BRAILLE_CMD_TAB (0x100 | 0x18)    /* fixture: space + dots 4+5 */
#define BRAILLE_CMD_PREV (0x100 | 0x04)   /* fixture: space + dot 3 */
#define BRAILLE_CMD_NEXT (0x100 | 0x20)   /* fixture: space + dot 6 */

#define BRAILLE_WHEN_ANY                                                                     \
    { BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY }

#define BRAILLE_KP_RULE(chord, usage, cls)                                                   \
    {                                                                                        \
        .chords = {chord}, .chord_count = 1,                                                 \
        .actions = {{.behavior_dev = DEVICE_DT_NAME(DT_NODELABEL(kp)),                       \
                     .param1 = BRAILLE_KEY_USAGE(usage)}},                                   \
        .action_count = 1, .when = BRAILLE_WHEN_ANY, .class = cls,                           \
    }

/* The fixture command set: the same chords in every mode for now, but each
 * mode owns its own array so phase 02 can diverge them independently.
 *   Braille space      -> &kp SPACE       (whitespace)
 *   space + dots 1+5   -> &kp RETURN      (whitespace)  fixture chord
 *   space + dots 4+5   -> &kp TAB         (whitespace)  fixture chord
 *   space + dot 3      -> &kp LEFT_ARROW  (navigation)  fixture chord
 *   space + dot 6      -> &kp RIGHT_ARROW (navigation)  fixture chord */
#define BRAILLE_FIXTURE_COMMAND_RULES                                                        \
    {                                                                                        \
        BRAILLE_KP_RULE(BRAILLE_CMD_SPACE, BRAILLE_KEY_SPACE, BRAILLE_CLASS_SPACE),          \
            BRAILLE_KP_RULE(BRAILLE_CMD_RETURN, BRAILLE_KEY_RETURN, BRAILLE_CLASS_NEUTRAL),  \
            BRAILLE_KP_RULE(BRAILLE_CMD_TAB, BRAILLE_KEY_TAB, BRAILLE_CLASS_NEUTRAL),        \
            BRAILLE_KP_RULE(BRAILLE_CMD_PREV, BRAILLE_KEY_LEFT_ARROW, BRAILLE_CLASS_NEUTRAL), \
            BRAILLE_KP_RULE(BRAILLE_CMD_NEXT, BRAILLE_KEY_RIGHT_ARROW, BRAILLE_CLASS_NEUTRAL), \
    }

#define BRAILLE_COMMAND_TABLE(name)                                                          \
    static const struct braille_rule name##_rules[] = BRAILLE_FIXTURE_COMMAND_RULES;         \
    const struct braille_rule_table braille_rules_##name##_commands = {                      \
        .rules = name##_rules,                                                               \
        .count = sizeof(name##_rules) / sizeof(name##_rules[0]),                             \
    };                                                                                       \
    const struct braille_whitespace_chords braille_commands_##name##_whitespace = {          \
        .space = BRAILLE_CMD_SPACE,                                                          \
        .enter = BRAILLE_CMD_RETURN,                                                         \
        .tab = BRAILLE_CMD_TAB,                                                              \
    }

BRAILLE_COMMAND_TABLE(ueb1);
BRAILLE_COMMAND_TABLE(computer8);
#ifdef CONFIG_ZMK_BRAILLE_GRADE2
BRAILLE_COMMAND_TABLE(ueb2);
#endif

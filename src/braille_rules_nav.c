// SPDX-License-Identifier: MIT

#include <dt-bindings/zmk/keys.h>
#include <braille_engine.h>
#include <braille_rules.h>
#include <braille_tables.h>

/*
 * Direct factory commands, verified in references/research/factory-command-chords.yaml.
 * Mac source: https://support.apple.com/en-us/103560, "Common braille commands
 * for VoiceOver on Mac" (Navigation, Scrolling, Braille).
 *
 * These emit ordinary host keys, not VoiceOver API commands. Raw arrows follow
 * the candidate catalog; Home/End are first/last-item analogues, NOT macOS
 * line-start/end shortcuts (compose sticky Command + Left/Right for those).
 * Each mode owns a separate array, validated with its character table before
 * selection. No settings-entry chord is assigned until Phase 05 has a UI.
 */

#define BRAILLE_KP_RULE(chord, key, cls)                                                     \
    {                                                                                      \
        .chords = {chord}, .chord_count = 1,                                                \
        .actions = {{.behavior_dev = DEVICE_DT_NAME(DT_NODELABEL(kp)), .param1 = key}},       \
        .action_count = 1, .class = cls,                                                    \
    }

#define BRAILLE_NAV_RULES                                                                   \
    /* Project space/navigation requirement: Braille space alone is literal space. */       \
    BRAILLE_KP_RULE(0x100, SPACE, BRAILLE_CLASS_SPACE),                                       \
    /* Mac VoiceOver, Navigation: previous item (space+1). */                                \
    BRAILLE_KP_RULE(0x101, LEFT_ARROW, BRAILLE_CLASS_NEUTRAL),                                \
    /* Mac VoiceOver, Navigation: next item (space+4). */                                    \
    BRAILLE_KP_RULE(0x108, RIGHT_ARROW, BRAILLE_CLASS_NEUTRAL),                               \
    /* Mac VoiceOver, Navigation: move up (space+3). */                                     \
    BRAILLE_KP_RULE(0x104, UP_ARROW, BRAILLE_CLASS_NEUTRAL),                                  \
    /* Mac VoiceOver, Navigation: move down (space+6). */                                   \
    BRAILLE_KP_RULE(0x120, DOWN_ARROW, BRAILLE_CLASS_NEUTRAL),                                \
    /* Mac VoiceOver, Navigation: first item (space+123). */                                \
    BRAILLE_KP_RULE(0x107, HOME, BRAILLE_CLASS_NEUTRAL),                                      \
    /* Mac VoiceOver, Navigation: last item (space+456). */                                 \
    BRAILLE_KP_RULE(0x138, END, BRAILLE_CLASS_NEUTRAL),                                       \
    /* Mac VoiceOver, Scrolling: up one page (space+3456). */                               \
    BRAILLE_KP_RULE(0x13c, PAGE_UP, BRAILLE_CLASS_NEUTRAL),                                   \
    /* Mac VoiceOver, Scrolling: down one page (space+1456). */                             \
    BRAILLE_KP_RULE(0x139, PAGE_DOWN, BRAILLE_CLASS_NEUTRAL),                                 \
    /* Mac VoiceOver, Braille: Tab is space+2345, never the old 0x118 fixture. */             \
    BRAILLE_KP_RULE(0x11e, TAB, BRAILLE_CLASS_NEUTRAL),                                       \
    /* Mac VoiceOver, Braille: Return (space+15). */                                        \
    BRAILLE_KP_RULE(0x111, RETURN, BRAILLE_CLASS_NEUTRAL)

#define BRAILLE_COMMAND_TABLE(name)                                                        \
    static const struct braille_rule name##_rules[] = {BRAILLE_NAV_RULES};                   \
    const struct braille_rule_table braille_rules_##name##_commands = {                     \
        .rules = name##_rules,                                                              \
        .count = sizeof(name##_rules) / sizeof(name##_rules[0]),                             \
    };                                                                                     \
    const struct braille_whitespace_chords braille_commands_##name##_whitespace = {         \
        .space = 0x100, .enter = 0x111, .tab = 0x11e,                                       \
    }

BRAILLE_COMMAND_TABLE(ueb1);
BRAILLE_COMMAND_TABLE(computer8);
#ifdef CONFIG_ZMK_BRAILLE_GRADE2
BRAILLE_COMMAND_TABLE(ueb2);
#endif

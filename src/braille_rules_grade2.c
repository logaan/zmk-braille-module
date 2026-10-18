// SPDX-License-Identifier: MIT

#include <braille_rules.h>
#include <braille_tables.h>

/* The optional `ueb2` character table, compiled only when
 * CONFIG_ZMK_BRAILLE_GRADE2 includes this unit: empty until phase 07 adds
 * the Grade 2 contractions here. */
const struct braille_rule_table braille_rules_ueb2_characters = {
    .rules = NULL,
    .count = 0,
};

// SPDX-License-Identifier: MIT

#include <braille_rules.h>
#include <braille_tables.h>

/* The `ueb1` character table: empty until the alphabet data task adds the
 * 26 six-dot cells here; phase 03 completes the Grade 1 literary signs. */
const struct braille_rule_table braille_rules_ueb1_characters = {
    .rules = NULL,
    .count = 0,
};

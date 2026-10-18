// SPDX-License-Identifier: MIT

#ifndef BRAILLE_TABLES_H
#define BRAILLE_TABLES_H

#include <stddef.h>

#include <braille_engine.h>
#include <braille_rules.h>

/*
 * Named character-table registration. Each compiled mode pairs one
 * character table with its own factory command table (navigation and
 * noncharacter functions), and the one shared user override layer stays
 * active in every mode. `ueb1` and `computer8` are always compiled; `ueb2`
 * is exposed only when its rules are included (CONFIG_ZMK_BRAILLE_GRADE2).
 * Future named language tables are further entries in the same
 * representation: capture, matching, and navigation do not change.
 *
 * `braille_tables_select` is the single selection mechanism: the boot-time
 * devicetree default (the zmk,braille node's `default-mode` property,
 * applied by `braille_tables_select_default`) and phase-05 settings changes
 * both select a compiled table by name through it.
 */

/* A compiled mode: a stable name, its character table, its own factory
 * command table, and the whitespace command chords (Braille space, Return,
 * Tab) that commit pending input as at a word end. All storage is static
 * const factory data. */
struct braille_mode {
    const char *name;
    const struct braille_rule_table *characters;
    const struct braille_rule_table *commands;
    const struct braille_whitespace_chords *whitespace;
};

/* Compiled factory tables, defined by the rule-data units. `ueb1` carries
 * the 26 six-dot alphabet cells; the remaining character tables are empty
 * until their data tasks populate them (phases 03/04/07). Per-mode command
 * tables carry the verified direct factory subset; see the source audit in
 * references/research/factory-command-chords.yaml for gaps and limitations. */
extern const struct braille_rule_table braille_rules_ueb1_characters;
extern const struct braille_rule_table braille_rules_computer8_characters;
extern const struct braille_rule_table braille_rules_ueb1_commands;
extern const struct braille_rule_table braille_rules_computer8_commands;
extern const struct braille_whitespace_chords braille_commands_ueb1_whitespace;
extern const struct braille_whitespace_chords braille_commands_computer8_whitespace;
#ifdef CONFIG_ZMK_BRAILLE_GRADE2
extern const struct braille_rule_table braille_rules_ueb2_characters;
extern const struct braille_rule_table braille_rules_ueb2_commands;
extern const struct braille_whitespace_chords braille_commands_ueb2_whitespace;
#endif

/* In-memory user override layer owned by the registry and re-registered
 * with the engine on every selection, so overrides stay active in every
 * mode. The matcher re-reads `count` on every chord, so rules appended here
 * take effect immediately. The capacity is a small fixture while phase 05
 * builds editing and persistence for the plan's 256 overrides. */
#ifndef BRAILLE_TABLES_OVERRIDE_CAPACITY
#define BRAILLE_TABLES_OVERRIDE_CAPACITY 8
#endif

/* A mutable view over the registry-owned override layer. The engine holds a
 * `struct braille_rule_table` view over the same storage and re-reads the
 * shared `count` on every chord, so rules appended here take effect
 * immediately. */
struct braille_override_layer {
    struct braille_rule *rules;
    size_t *count;
    size_t capacity;
};
struct braille_override_layer braille_tables_overrides(void);

/* Registry iteration, for the phase-05 settings UI and tests. Indexes are
 * 0..braille_tables_count(); out-of-range returns NULL. */
size_t braille_tables_count(void);
const struct braille_mode *braille_tables_mode(size_t index);

/* Exact, case-sensitive name lookup; NULL when no compiled table has the
 * name (including `ueb2` when its rules are not compiled in). */
const struct braille_mode *braille_tables_find(const char *name);

/* The single selection mechanism: select exactly one compiled character
 * table by name, paired with its own factory command table. Both factory
 * tables are validated together; a colliding mode is rejected and the
 * previous selection kept. Selection discards pending input and resets the
 * UEB context (via braille_engine_set_tables). The override layer stays
 * active. Returns 0, -ENOENT for an uncompiled name, or -EINVAL on a
 * factory collision. */
int braille_tables_select(const char *name);

/* Select the devicetree default mode (the zmk,braille node's `default-mode`
 * property, "ueb1" when the node is absent; -DBRAILLE_TABLES_DEFAULT_MODE
 * overrides both in host tests). Same mechanism as settings changes. */
int braille_tables_select_default(void);

/* The selected mode's name, or NULL before the first selection. */
const char *braille_tables_active(void);

#endif /* BRAILLE_TABLES_H */

// SPDX-License-Identifier: MIT
/*
 * Host tests for the named character-table registry (src/braille_tables.c)
 * and its factory command tables (src/braille_rules_nav.c), compiled
 * against the stub ZMK headers and run natively by test_tables.py.
 *
 * The registry is exercised through the real compiled tables: the (still
 * partial) ueb1/computer8/ueb2 character tables, each mode's own factory
 * command table, and the registry-owned in-memory override layer. The
 * matcher's commit hook is overridden here to record commits, so selection
 * is verified by feeding chords through braille_chord_resolved.
 *
 * Built twice by test_tables.py: without and with
 * -DCONFIG_ZMK_BRAILLE_GRADE2 (ueb2 exposed only when included).
 */
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <braille_chord.h>
#include <braille_engine.h>
#include <braille_rules.h>
#include <braille_tables.h>
#include <zmk/behavior.h>

/* --- Commit log ------------------------------------------------------------ */

static const struct braille_rule *commits[16];
static enum braille_commit_cause causes[16];
static size_t commit_count;

static void log_reset(void) { commit_count = 0; }

int braille_engine_commit_rule(const struct braille_rule *rule, enum braille_commit_cause cause,
                               struct zmk_behavior_binding_event event) {
    (void)event;
    assert(commit_count < sizeof(commits) / sizeof(commits[0]));
    commits[commit_count] = rule;
    causes[commit_count] = cause;
    commit_count++;
    return 0;
}

static const struct zmk_behavior_binding_event ev = {.layer = 0, .position = 3, .timestamp = 1};

static void chord(uint16_t mask) {
    int ret = braille_chord_resolved(mask, ev);
    assert(ret == 0);
}

/* Mac VoiceOver factory command chords. */
#define CMD_SPACE 0x100
#define CMD_RETURN 0x111
#define CMD_TAB 0x11e
#define CMD_PREV 0x101
#define CMD_NEXT 0x108
#define DOT_A 0x001 /* dot 1: the ueb1 `a` cell; the override shadows it */

/* 1. Exactly the compiled tables are registered by name: `ueb1` and
 *    `computer8` always, `ueb2` only when its rules are included. */
static void test_registration(void) {
#ifdef CONFIG_ZMK_BRAILLE_GRADE2
    assert(braille_tables_count() == 3);
    assert(braille_tables_find("ueb2") != NULL);
#else
    assert(braille_tables_count() == 2);
    assert(braille_tables_find("ueb2") == NULL);
#endif
    assert(braille_tables_find("ueb1") != NULL);
    assert(braille_tables_find("computer8") != NULL);
    assert(braille_tables_find("japanese6") == NULL); /* no non-compiled table */
    assert(braille_tables_find("UEB1") == NULL);      /* exact, case-sensitive */
    assert(braille_tables_find(NULL) == NULL);
    assert(braille_tables_mode(0) == braille_tables_find("ueb1"));
    assert(braille_tables_mode(1) == braille_tables_find("computer8"));
    assert(braille_tables_mode(braille_tables_count()) == NULL);

    /* Selecting a name that is not compiled in is rejected. */
    assert(braille_tables_select("ueb2-not-compiled") == -ENOENT);
#ifndef CONFIG_ZMK_BRAILLE_GRADE2
    assert(braille_tables_select("ueb2") == -ENOENT);
#endif
    assert(braille_tables_active() == NULL); /* nothing selected yet */
    printf("ok: registration by name\n");
}

/* 2. Selecting each compiled table by name pairs it with its own factory
 *    command table: navigation and noncharacter functions work in every
 *    mode, and each mode's command rules are its own storage. */
static void test_mode_command_tables(void) {
    const struct braille_mode *ueb1 = braille_tables_find("ueb1");
    const struct braille_mode *computer8 = braille_tables_find("computer8");
    assert(ueb1->commands == &braille_rules_ueb1_commands);
    assert(computer8->commands == &braille_rules_computer8_commands);
    assert(ueb1->commands != computer8->commands); /* its own command table */
    assert(ueb1->characters == &braille_rules_ueb1_characters);
    assert(computer8->characters == &braille_rules_computer8_characters);
#ifdef CONFIG_ZMK_BRAILLE_GRADE2
    const struct braille_mode *ueb2 = braille_tables_find("ueb2");
    assert(ueb2->commands == &braille_rules_ueb2_commands);
    assert(ueb2->characters == &braille_rules_ueb2_characters);
#endif

    for (size_t i = 0; i < braille_tables_count(); ++i) {
        const struct braille_mode *mode = braille_tables_mode(i);
        assert(braille_tables_select(mode->name) == 0);
        assert(braille_tables_active() == mode->name);
        log_reset();

        /* Navigation and noncharacter functions are available in this mode:
         * each factory command rule commits through the shared engine. */
        chord(CMD_PREV);
        chord(CMD_NEXT);
        chord(CMD_RETURN);
        assert(commit_count == 3);
        assert(commits[0]->chords[0] == CMD_PREV);
        assert(commits[0]->actions[0].param1 == ((0x07UL << 16) | 0x50));
        assert(commits[1]->chords[0] == CMD_NEXT);
        assert(commits[1]->actions[0].param1 == ((0x07UL << 16) | 0x4f));
        assert(commits[2]->chords[0] == CMD_RETURN);
        assert(commits[2]->actions[0].param1 == ((0x07UL << 16) | 0x28));

        /* The mode's whitespace chords are selected with it. */
        assert(braille_engine_is_whitespace(CMD_SPACE));
        assert(braille_engine_is_whitespace(CMD_RETURN));
        assert(braille_engine_is_whitespace(CMD_TAB));
        assert(!braille_engine_is_whitespace(CMD_PREV));
        assert(!braille_engine_is_whitespace(DOT_A));
    }
    printf("ok: each mode selects its own command table\n");
}

/* 3. The user override layer stays active in every mode: one shared layer,
 *    winning over factory rules and surviving mode switches. */
static void test_overrides_active_in_every_mode(void) {
    struct braille_override_layer overrides = braille_tables_overrides();
    assert(overrides.rules != NULL && overrides.capacity >= 2);
    overrides.rules[0] = (struct braille_rule){
        .chords = {DOT_A},
        .chord_count = 1,
        .actions = {{.behavior_dev = "&macro", .param1 = 0x0a}},
        .action_count = 1,
        .when = {BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY},
        .class = BRAILLE_CLASS_NEUTRAL,
    };
    *overrides.count = 1;
    const struct braille_rule *ovr = &overrides.rules[0];

    for (size_t i = 0; i < braille_tables_count(); ++i) {
        assert(braille_tables_select(braille_tables_mode(i)->name) == 0);
        log_reset();
        chord(DOT_A);
        assert(commit_count == 1);
        assert(commits[0] == ovr); /* the override, not a factory rule */
        assert(causes[0] == BRAILLE_COMMIT_CLEAN);
    }

    /* The override wins over a factory rule with the same input: rebind the
     * navigation chord and the factory LEFT_ARROW is shadowed. */
    overrides.rules[1] = (struct braille_rule){
        .chords = {CMD_PREV},
        .chord_count = 1,
        .actions = {{.behavior_dev = "&macro", .param1 = 0x0b}},
        .action_count = 1,
        .when = {BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY},
        .class = BRAILLE_CLASS_NEUTRAL,
    };
    *overrides.count = 2;
    for (size_t i = 0; i < braille_tables_count(); ++i) {
        assert(braille_tables_select(braille_tables_mode(i)->name) == 0);
        log_reset();
        chord(CMD_PREV);
        assert(commit_count == 1);
        assert(commits[0] == &overrides.rules[1]);
    }
    *overrides.count = 0;
    printf("ok: overrides active in every mode\n");
}

/* 4. The devicetree default uses the same selection mechanism: the
 *    compiled-in default mode name selects like any other. */
static void test_devicetree_default_selection(void) {
    /* An uncompiled default is reported, not silently ignored (verified by
     * the -DBRAILLE_TABLES_DEFAULT_MODE=\"ueb2\" build without Grade 2). */
#ifdef BRAILLE_TABLES_TEST_DEFAULT_UNCOMPILED
    assert(braille_tables_select_default() == -ENOENT);
#elif defined(BRAILLE_TABLES_TEST_DEFAULT)
    assert(braille_tables_select_default() == 0);
    assert(strcmp(braille_tables_active(), BRAILLE_TABLES_TEST_DEFAULT) == 0);
#else
    assert(braille_tables_select_default() == 0);
    assert(strcmp(braille_tables_active(), "ueb1") == 0);
#endif
    printf("ok: devicetree default selection\n");
}

/* 5. A mode switch discards pending input: a pending prefix held under one
 *    mode does not leak into the next. Uses the override layer to create a
 *    two-chord rule whose longer sequence holds the single-cell factory `a`
 *    pending. */
static void test_mode_switch_resets_pending(void) {
    struct braille_override_layer overrides = braille_tables_overrides();
    overrides.rules[0] = (struct braille_rule){
        .chords = {DOT_A, DOT_A},
        .chord_count = 2,
        .actions = {{.behavior_dev = "&macro", .param1 = 0xaa}},
        .action_count = 1,
        .when = {BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY},
        .class = BRAILLE_CLASS_NEUTRAL,
    };
    *overrides.count = 1;

    assert(braille_tables_select("ueb1") == 0);
    log_reset();
    chord(DOT_A);
    assert(braille_engine_pending_count() == 1); /* held: [a a] may extend */
    assert(braille_tables_select("computer8") == 0);
    assert(braille_engine_pending_count() == 0); /* discarded by the switch */
    assert(commit_count == 0);                   /* no host output, no notice */

    chord(DOT_A);
    assert(braille_engine_pending_count() == 1); /* a fresh sequence */
    chord(DOT_A);
    assert(commit_count == 1);
    assert(commits[0] == &overrides.rules[0]);
    *overrides.count = 0;
    printf("ok: mode switch discards pending input\n");
}

int main(void) {
    test_registration();
    test_mode_command_tables();
    test_overrides_active_in_every_mode();
    test_devicetree_default_selection();
    test_mode_switch_resets_pending();
    printf("all table registry tests passed\n");
    return 0;
}

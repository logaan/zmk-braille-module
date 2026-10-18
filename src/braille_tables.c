// SPDX-License-Identifier: MIT

#include <errno.h>
#include <string.h>

#include <braille_engine.h>
#include <braille_tables.h>

#ifdef __ZEPHYR__
#include <zephyr/devicetree.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(braille_tables, CONFIG_ZMK_LOG_LEVEL);
#endif

/*
 * The registry of compiled named character tables. Each entry pairs one
 * character table with its own factory command table and whitespace chords;
 * the shared user override layer is registered with the engine alongside
 * every selection, so navigation, noncharacter functions, and overrides are
 * available in every mode. Adding a future named language table is a new
 * entry here plus its rule-data unit; capture, matching, and navigation are
 * unchanged.
 */

/* The devicetree default mode: the zmk,braille node's `default-mode`
 * property, "ueb1" when the node is absent. */
#ifndef BRAILLE_TABLES_DEFAULT_MODE
#ifdef __ZEPHYR__
#if DT_NODE_EXISTS(DT_NODELABEL(braille))
#define BRAILLE_TABLES_DEFAULT_MODE DT_PROP(DT_NODELABEL(braille), default_mode)
#else
#define BRAILLE_TABLES_DEFAULT_MODE "ueb1"
#endif
#else
#define BRAILLE_TABLES_DEFAULT_MODE "ueb1"
#endif
#endif

static const struct braille_mode modes[] = {
    {
        .name = "ueb1",
        .characters = &braille_rules_ueb1_characters,
        .commands = &braille_rules_ueb1_commands,
        .whitespace = &braille_commands_ueb1_whitespace,
    },
    {
        .name = "computer8",
        .characters = &braille_rules_computer8_characters,
        .commands = &braille_rules_computer8_commands,
        .whitespace = &braille_commands_computer8_whitespace,
    },
#ifdef CONFIG_ZMK_BRAILLE_GRADE2
    {
        .name = "ueb2",
        .characters = &braille_rules_ueb2_characters,
        .commands = &braille_rules_ueb2_commands,
        .whitespace = &braille_commands_ueb2_whitespace,
    },
#endif
};

/* The one user override layer, active in every mode. */
static struct braille_rule override_storage[BRAILLE_TABLES_OVERRIDE_CAPACITY];
static struct braille_rule_table override_layer = {
    .rules = override_storage,
    .count = 0,
};

static const struct braille_mode *active_mode;

struct braille_override_layer braille_tables_overrides(void) {
    return (struct braille_override_layer){
        .rules = override_storage,
        .count = &override_layer.count,
        .capacity = BRAILLE_TABLES_OVERRIDE_CAPACITY,
    };
}

size_t braille_tables_count(void) { return sizeof(modes) / sizeof(modes[0]); }

const struct braille_mode *braille_tables_mode(size_t index) {
    return index < braille_tables_count() ? &modes[index] : NULL;
}

const struct braille_mode *braille_tables_find(const char *name) {
    if (!name) {
        return NULL;
    }
    for (size_t i = 0; i < braille_tables_count(); ++i) {
        if (strcmp(modes[i].name, name) == 0) {
            return &modes[i];
        }
    }
    return NULL;
}

const char *braille_tables_active(void) { return active_mode ? active_mode->name : NULL; }

int braille_tables_select(const char *name) {
    const struct braille_mode *mode = braille_tables_find(name);
    if (!mode) {
        return -ENOENT;
    }
    /* Both factory tables are validated together; a colliding mode is
     * rejected here and the engine keeps the previous selection. */
    int ret = braille_engine_set_tables(mode->characters, mode->commands);
    if (ret < 0) {
        return ret;
    }
    braille_engine_set_whitespace(mode->whitespace);
    /* Re-register the one override layer so it stays active in every mode. */
    braille_engine_set_overrides(&override_layer);
    active_mode = mode;
    return 0;
}

int braille_tables_select_default(void) {
    return braille_tables_select(BRAILLE_TABLES_DEFAULT_MODE);
}

#ifdef __ZEPHYR__
/* Boot-time selection: the module is always on, so the devicetree default
 * mode (the zmk,braille node's `default-mode` property, "ueb1" when the node
 * is absent) is selected once at startup, before any chord can resolve.
 * Selection only retargets table views and validates them; no behavior device
 * is invoked here. A failure leaves the engine without factory tables, so the
 * error is logged rather than asserted. */
static int braille_tables_boot(void) {
    int ret = braille_tables_select_default();
    if (ret < 0) {
        LOG_ERR("default Braille mode \"%s\" not selected: %d", BRAILLE_TABLES_DEFAULT_MODE, ret);
    }
    return 0;
}

SYS_INIT(braille_tables_boot, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
#endif

// SPDX-License-Identifier: MIT
#include <errno.h>
#include <string.h>
#include <drivers/behavior.h>
#include <braille_chord.h>
#include <braille_tables.h>
#include <dt-bindings/zmk/braille.h>

#if DT_HAS_COMPAT_STATUS_OKAY(zmk_behavior_braille_select)
static int select_pressed(struct zmk_behavior_binding *binding,
                          struct zmk_behavior_binding_event event) {
    (void)event;
    if (binding->param2 || binding->param1 > BRAILLE_MODE_NEXT) {
        return -EINVAL;
    }
    const char *name = NULL;
    switch (binding->param1) {
    case BRAILLE_MODE_UEB1: name = "ueb1"; break;
    case BRAILLE_MODE_COMPUTER8: name = "computer8"; break;
    case BRAILLE_MODE_UEB2: name = "ueb2"; break;
    case BRAILLE_MODE_NEXT: {
        size_t count = braille_tables_count();
        if (!count) { return -ENOENT; }
        size_t next = 0;
        const char *active = braille_tables_active();
        for (size_t i = 0; active && i < count; ++i) {
            if (strcmp(active, braille_tables_mode(i)->name) == 0) {
                next = (i + 1) % count;
                break;
            }
        }
        name = braille_tables_mode(next)->name;
        break;
    }
    }
    int ret = braille_tables_select(name);
    if (!ret) {
        /* Preserve held ownership until idle, but never translate a partial
         * old-mode chord using the new mode. Selection resets engine context. */
        braille_chord_discard();
    }
    return ret;
}

static int select_released(struct zmk_behavior_binding *binding,
                           struct zmk_behavior_binding_event event) {
    (void)event;
    return binding->param2 || binding->param1 > BRAILLE_MODE_NEXT ? -EINVAL : 0;
}

#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
static const struct behavior_parameter_value_metadata select_values[] = {
    {.display_name = "UEB1=0, Computer8=1, UEB2=2, Next=3",
     .type = BEHAVIOR_PARAMETER_VALUE_TYPE_RANGE,
     .range = {.min = BRAILLE_MODE_UEB1, .max = BRAILLE_MODE_NEXT}},
};
static const struct behavior_parameter_metadata_set select_sets[] = {
    {.param1_values_len = ARRAY_SIZE(select_values), .param1_values = select_values},
};
static const struct behavior_parameter_metadata select_metadata = {
    .sets_len = ARRAY_SIZE(select_sets), .sets = select_sets,
};
#endif
static const struct behavior_driver_api select_api = {
    .locality = BEHAVIOR_LOCALITY_CENTRAL,
    .binding_pressed = select_pressed,
    .binding_released = select_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .parameter_metadata = &select_metadata,
#endif
};
#define SELECT_DEFINE(node) \
    BEHAVIOR_DT_DEFINE(node, NULL, NULL, NULL, NULL, POST_KERNEL, \
                       CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &select_api);
DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_select, SELECT_DEFINE)
#endif

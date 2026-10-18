// SPDX-License-Identifier: MIT

#include <errno.h>
#include <drivers/behavior.h>
#include <braille_chord.h>

/*
 * Only Braille behavior bindings enter the detector: no position/layer filter,
 * global key listener, mode toggle, timer, or character/command dispatch.
 * ZMK's keymap retains the press-time layer state for release routing, so an
 * ordinary layer change must not cause us to re-resolve the current binding.
 */
#define BRAILLE_DEFINE(node_id, api)                                                               \
    BEHAVIOR_DT_DEFINE(node_id, NULL, NULL, NULL, NULL, POST_KERNEL,                               \
                       CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, api);

#if DT_HAS_COMPAT_STATUS_OKAY(zmk_behavior_braille_dot)

static int braille_dot_input(struct zmk_behavior_binding *binding,
                             struct zmk_behavior_binding_event event, bool pressed) {
    /* Also validate dynamic/rule bindings, before subtracting or shifting. */
    if (binding->param1 < 1 || binding->param1 > 8 || binding->param2 != 0) {
        return -EINVAL;
    }

    return braille_chord_key_event((uint16_t)(1U << (binding->param1 - 1)), pressed, event);
}

static int braille_dot_pressed(struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event) {
    return braille_dot_input(binding, event, true);
}

static int braille_dot_released(struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event) {
    return braille_dot_input(binding, event, false);
}

#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
static const struct behavior_parameter_value_metadata dot_values[] = {
    {.display_name = "Dot",
     .type = BEHAVIOR_PARAMETER_VALUE_TYPE_RANGE,
     .range = {.min = 1, .max = 8}},
};
static const struct behavior_parameter_metadata_set dot_metadata_sets[] = {
    {.param1_values_len = ARRAY_SIZE(dot_values), .param1_values = dot_values},
};
static const struct behavior_parameter_metadata dot_metadata = {
    .sets_len = ARRAY_SIZE(dot_metadata_sets),
    .sets = dot_metadata_sets,
};
#endif

static const struct behavior_driver_api braille_dot_driver_api = {
    .locality = BEHAVIOR_LOCALITY_CENTRAL,
    .binding_pressed = braille_dot_pressed,
    .binding_released = braille_dot_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .parameter_metadata = &dot_metadata,
#endif
};

#define BRAILLE_DOT_DEFINE(node_id) BRAILLE_DEFINE(node_id, &braille_dot_driver_api)
DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_dot, BRAILLE_DOT_DEFINE)

#endif

#if DT_HAS_COMPAT_STATUS_OKAY(zmk_behavior_braille_space)

static int braille_space_input(struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event, bool pressed) {
    if (binding->param1 != 0 || binding->param2 != 0) {
        return -EINVAL;
    }

    return braille_chord_key_event(BRAILLE_CHORD_SPACE_BIT, pressed, event);
}

static int braille_space_pressed(struct zmk_behavior_binding *binding,
                                 struct zmk_behavior_binding_event event) {
    return braille_space_input(binding, event, true);
}

static int braille_space_released(struct zmk_behavior_binding *binding,
                                  struct zmk_behavior_binding_event event) {
    return braille_space_input(binding, event, false);
}

static const struct behavior_driver_api braille_space_driver_api = {
    .locality = BEHAVIOR_LOCALITY_CENTRAL,
    .binding_pressed = braille_space_pressed,
    .binding_released = braille_space_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif
};

#define BRAILLE_SPACE_DEFINE(node_id) BRAILLE_DEFINE(node_id, &braille_space_driver_api)
DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_space, BRAILLE_SPACE_DEFINE)

#endif

/* The later mode/settings leaf owns these paths; never feed them as bits. */
#if DT_HAS_COMPAT_STATUS_OKAY(zmk_behavior_braille_mode) ||                                        \
    DT_HAS_COMPAT_STATUS_OKAY(zmk_behavior_braille_settings)

static int braille_binding_unsupported(struct zmk_behavior_binding *binding,
                                       struct zmk_behavior_binding_event event) {
    (void)binding;
    (void)event;
    return -ENOTSUP;
}

static const struct behavior_driver_api braille_unsupported_driver_api = {
    .locality = BEHAVIOR_LOCALITY_CENTRAL,
    .binding_pressed = braille_binding_unsupported,
    .binding_released = braille_binding_unsupported,
};

#define BRAILLE_UNSUPPORTED_DEFINE(node_id) BRAILLE_DEFINE(node_id, &braille_unsupported_driver_api)
DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_mode, BRAILLE_UNSUPPORTED_DEFINE)
DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_settings, BRAILLE_UNSUPPORTED_DEFINE)

#endif

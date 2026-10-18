// SPDX-License-Identifier: MIT

#include <errno.h>
#include <drivers/behavior.h>

/*
 * Build/link scaffolding only. Binding schemas and default nodes are supplied
 * separately; capture, mode switching and settings entry are not implemented.
 * Do not silently report successful input before those paths exist.
 */
#if DT_HAS_COMPAT_STATUS_OKAY(zmk_behavior_braille_dot) ||                                             \
    DT_HAS_COMPAT_STATUS_OKAY(zmk_behavior_braille_space) ||                                           \
    DT_HAS_COMPAT_STATUS_OKAY(zmk_behavior_braille_mode) ||                                            \
    DT_HAS_COMPAT_STATUS_OKAY(zmk_behavior_braille_settings)

static int braille_binding_unsupported(struct zmk_behavior_binding *binding,
                                       struct zmk_behavior_binding_event event) {
    (void)binding;
    (void)event;
    return -ENOTSUP;
}

static const struct behavior_driver_api braille_driver_api = {
    .locality = BEHAVIOR_LOCALITY_CENTRAL,
    .binding_pressed = braille_binding_unsupported,
    .binding_released = braille_binding_unsupported,
};

#define BRAILLE_DEFINE(node_id)                                                                    \
    BEHAVIOR_DT_DEFINE(node_id, NULL, NULL, NULL, NULL, POST_KERNEL,                                 \
                       CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &braille_driver_api);

DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_dot, BRAILLE_DEFINE)
DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_space, BRAILLE_DEFINE)
DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_mode, BRAILLE_DEFINE)
DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_settings, BRAILLE_DEFINE)

#endif

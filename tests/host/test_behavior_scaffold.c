// SPDX-License-Identifier: MIT
#include <assert.h>
#include "../../src/behavior_braille.c"

#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
int zmk_behavior_get_empty_param_metadata(const struct device *dev,
                                          struct behavior_parameter_metadata *metadata) {
    (void)dev;
    *metadata = (struct behavior_parameter_metadata){0};
    return 0;
}
#endif

int main(void) {
    /* The real downstream placeholder must not silently claim working capture. */
    struct zmk_behavior_binding binding = {.behavior_dev = "test", .param1 = 8};
    struct zmk_behavior_binding_event event = {.position = 42, .layer = 3, .timestamp = 1234};
#if TEST_DOT_NODES
    assert(dot_a->locality == BEHAVIOR_LOCALITY_CENTRAL);
    assert(dot_b->locality == BEHAVIOR_LOCALITY_CENTRAL);
    assert(dot_a->binding_pressed(&binding, event) == -ENOTSUP);
    assert(dot_b->binding_released(&binding, event) == -ENOTSUP);
    assert(binding.param1 == 8 && binding.param2 == 0);
#endif
    binding.param1 = 0;
#if TEST_SPACE_NODES
    assert(space->locality == BEHAVIOR_LOCALITY_CENTRAL);
    assert(space->binding_pressed(&binding, event) == -ENOTSUP);
    assert(space->binding_released(&binding, event) == -ENOTSUP);
#endif
#if TEST_MODE_NODES
    assert(mode->locality == BEHAVIOR_LOCALITY_CENTRAL);
    assert(mode->binding_pressed(&binding, event) == -ENOTSUP);
    assert(mode->binding_released(&binding, event) == -ENOTSUP);
#endif
#if TEST_SETTINGS_NODES
    assert(settings->locality == BEHAVIOR_LOCALITY_CENTRAL);
    assert(settings->binding_pressed(&binding, event) == -ENOTSUP);
    assert(settings->binding_released(&binding, event) == -ENOTSUP);
#endif
    (void)binding;
    (void)event;
    return 0;
}

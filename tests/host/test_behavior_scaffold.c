// SPDX-License-Identifier: MIT
#include <assert.h>
#include "../../src/behavior_braille.c"

/* Host-only forwarding stub; real hold-tap semantics run in native ZMK. */
int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event, bool pressed) {
    (void)binding;
    (void)event;
    (void)pressed;
    return 0;
}

#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
int zmk_behavior_get_empty_param_metadata(const struct device *dev,
                                          struct behavior_parameter_metadata *metadata) {
    (void)dev;
    *metadata = (struct behavior_parameter_metadata){0};
    return 0;
}
#endif

int main(void) {
    /* Capture works; the unavailable completion consumer remains explicit. */
    struct zmk_behavior_binding binding = {.behavior_dev = "test", .param1 = 8};
    struct zmk_behavior_binding_event event = {.position = 42, .layer = 3, .timestamp = 1234};
#if TEST_DOT_NODES
    assert(dot_a->locality == BEHAVIOR_LOCALITY_CENTRAL);
    assert(dot_b->locality == BEHAVIOR_LOCALITY_CENTRAL);
    assert(dot_a->binding_pressed(&binding, event) == 0);
    assert(dot_b->binding_released(&binding, event) == -ENOTSUP);
    assert(binding.param1 == 8 && binding.param2 == 0);
#endif
    binding.param1 = 0;
#if TEST_SPACE_NODES
    assert(space->locality == BEHAVIOR_LOCALITY_CENTRAL);
    assert(space->binding_pressed(&binding, event) == 0);
    assert(space->binding_released(&binding, event) == -ENOTSUP);
#endif
#if TEST_MODE_NODES
    assert(mode->locality == BEHAVIOR_LOCALITY_CENTRAL);
    assert(mode->binding_pressed(&binding, event) == 0);
    assert(mode->binding_released(&binding, event) == 0);
#endif
#if TEST_SETTINGS_NODES
    assert(settings->locality == BEHAVIOR_LOCALITY_CENTRAL);
    assert(settings->binding_pressed(&binding, event) == 0);
    assert(braille_settings_active());
    assert(settings->binding_released(&binding, event) == 0);
    assert(braille_settings_active());
    assert(braille_settings_exit(event) == 0);
#endif
    (void)binding;
    (void)event;
    return 0;
}

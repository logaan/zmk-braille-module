// SPDX-License-Identifier: MIT
#include <assert.h>
#include "../../src/behavior_braille.c"

int main(void) {
#if !TEST_NO_NODES
    const struct behavior_driver_api *devices[] = {dot_a, dot_b, space, mode, settings};
    struct zmk_behavior_binding binding = {.behavior_dev = "test", .param1 = 8, .param2 = 0};
    struct zmk_behavior_binding_event event = {.position = 42, .layer = 3, .timestamp = 1234};

    for (size_t i = 0; i < sizeof(devices) / sizeof(devices[0]); ++i) {
        assert(devices[i]->locality == BEHAVIOR_LOCALITY_CENTRAL);
        assert(devices[i]->binding_pressed(&binding, event) == -ENOTSUP);
        assert(devices[i]->binding_released(&binding, event) == -ENOTSUP);
        assert(binding.param1 == 8 && binding.param2 == 0);
    }
#endif
    return 0;
}

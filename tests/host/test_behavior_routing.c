// SPDX-License-Identifier: MIT
#include <assert.h>
#include <limits.h>
#include <string.h>
#include "../../src/behavior_braille.c"

/* Edge spy only: no held-key state, chord finalization, matching, or output. */
static struct {
    unsigned calls;
    uint16_t bit;
    bool pressed;
    struct zmk_behavior_binding_event event;
    int result;
} sink;

int braille_chord_key_event(uint16_t bit, bool pressed, struct zmk_behavior_binding_event event) {
    ++sink.calls;
    sink.bit = bit;
    sink.pressed = pressed;
    sink.event = event;
    return sink.result;
}

static unsigned forwarded;
static int forward_result;
int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event, bool pressed) {
    assert(strcmp(binding->behavior_dev, "braille_mode_hold_tap") == 0);
    assert(binding->param1 == 5 && binding->param2 == 5);
    assert(event.position == 0 && event.layer == 1);
    assert(pressed == (forwarded % 2 == 0));
    ++forwarded;
    return forward_result;
}
void braille_chord_discard(void) {}
int braille_chord_resolved(uint16_t mask, struct zmk_behavior_binding_event event) {
    (void)mask;
    (void)event;
    assert(false); /* Settings must not route to the host consumer. */
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

static void check_edge(const struct behavior_driver_api *api, uint32_t param, uint16_t bit,
                       bool pressed, struct zmk_behavior_binding_event event) {
    struct zmk_behavior_binding binding = {.behavior_dev = "test", .param1 = param};
    struct zmk_behavior_binding before = binding;
    unsigned calls = sink.calls;
    int ret =
        pressed ? api->binding_pressed(&binding, event) : api->binding_released(&binding, event);
    assert(ret == sink.result);
    assert(sink.calls == calls + 1);
    assert(sink.bit == bit && (sink.bit & ~BRAILLE_CHORD_MASK) == 0);
    assert(sink.pressed == pressed);
    assert(sink.event.layer == event.layer);
    assert(sink.event.position == event.position);
    assert(sink.event.timestamp == event.timestamp);
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
    assert(sink.event.source == event.source);
#endif
    assert(memcmp(&binding, &before, sizeof(binding)) == 0);
}

static void test_routing(void) {
    const struct behavior_driver_api *dots[] = {dot_a, dot_b};
    const uint32_t positions[] = {0, 1, 7, 15, 35, 99, UINT32_MAX};
    for (int layer = 0; layer < 32; ++layer) {
        for (size_t p = 0; p < ARRAY_SIZE(positions); ++p) {
            struct zmk_behavior_binding_event event = {
                .layer = layer,
                .position = positions[p],
                .timestamp = INT64_C(9876543210),
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
                .source = (uint8_t)(p % 2),
#endif
            };
            for (size_t d = 0; d < ARRAY_SIZE(dots); ++d) {
                assert(dots[d]->locality == BEHAVIOR_LOCALITY_CENTRAL);
                for (uint32_t dot = 1; dot <= 8; ++dot) {
                    check_edge(dots[d], dot, (uint16_t)(1U << (dot - 1)), true, event);
                    ++event.timestamp;
                    check_edge(dots[d], dot, (uint16_t)(1U << (dot - 1)), false, event);
                }
            }
            assert(space->locality == BEHAVIOR_LOCALITY_CENTRAL);
            check_edge(space, 0, UINT16_C(0x100), true, event);
            check_edge(space, 0, UINT16_C(0x100), false, event);
        }
    }
    /* A detector failure must remain visible, not become a successful key. */
    sink.result = -EIO;
    struct zmk_behavior_binding_event event = {.position = 3, .layer = 2};
    check_edge(dot_a, 4, UINT16_C(0x08), true, event);
    check_edge(dot_a, 4, UINT16_C(0x08), false, event);
    check_edge(space, 0, UINT16_C(0x100), true, event);
    check_edge(space, 0, UINT16_C(0x100), false, event);
    sink.result = 0;
}

static void test_invalid_parameters(void) {
    struct zmk_behavior_binding binding = {.behavior_dev = "test"};
    struct zmk_behavior_binding_event event = {.position = 42, .layer = 3};
    const uint32_t invalid_dots[] = {0, 9, 31, 32, 256, UINT32_MAX};
    unsigned calls = sink.calls;
    for (size_t i = 0; i < ARRAY_SIZE(invalid_dots); ++i) {
        binding.param1 = invalid_dots[i];
        assert(dot_a->binding_pressed(&binding, event) == -EINVAL);
        assert(dot_b->binding_released(&binding, event) == -EINVAL);
    }
    binding.param1 = 1;
    binding.param2 = 1;
    assert(dot_a->binding_pressed(&binding, event) == -EINVAL);
    assert(dot_a->binding_released(&binding, event) == -EINVAL);
    binding.param1 = 0;
    assert(space->binding_pressed(&binding, event) == -EINVAL);
    assert(space->binding_released(&binding, event) == -EINVAL);
    binding.param1 = 1;
    binding.param2 = 0;
    assert(space->binding_pressed(&binding, event) == -EINVAL);
    assert(space->binding_released(&binding, event) == -EINVAL);
    assert(sink.calls == calls);
}

static void test_non_chord_behaviors(void) {
    struct zmk_behavior_binding binding = {.behavior_dev = "test", .param1 = 5};
    struct zmk_behavior_binding_event event = {.position = 0, .layer = 1};
    unsigned calls = sink.calls;
    assert(mode->locality == BEHAVIOR_LOCALITY_CENTRAL);
    assert(mode->binding_pressed(&binding, event) == 0);
    assert(mode->binding_released(&binding, event) == 0);
    forward_result = -EIO;
    assert(mode->binding_pressed(&binding, event) == -EIO);
    assert(mode->binding_released(&binding, event) == -EIO);
    assert(forwarded == 4);
    binding.param1 = 32;
    assert(mode->binding_pressed(&binding, event) == -EINVAL);
    binding.param1 = 5;
    binding.param2 = 1;
    assert(mode->binding_released(&binding, event) == -EINVAL);
    assert(forwarded == 4);
    assert(settings->locality == BEHAVIOR_LOCALITY_CENTRAL);
    assert(settings->binding_pressed(&binding, event) == -EINVAL);
    assert(settings->binding_released(&binding, event) == -EINVAL);
    binding.param1 = binding.param2 = 0;
    assert(settings->binding_pressed(&binding, event) == 0);
    assert(braille_settings_active());
    assert(settings->binding_released(&binding, event) == 0);
    assert(braille_settings_active());
    assert(braille_settings_exit(event) == 0);
    assert(!braille_settings_active());
    assert(sink.calls == calls);
    /* Conventional &kp and layer keys use real ZMK in the native fixture. */
}

static void test_metadata(void) {
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    const struct behavior_parameter_metadata *metadata = dot_a->parameter_metadata;
    assert(metadata && metadata->sets_len == 1);
    const struct behavior_parameter_metadata_set *set = metadata->sets;
    assert(set->param1_values_len == 1 && set->param2_values_len == 0);
    assert(set->param1_values[0].type == BEHAVIOR_PARAMETER_VALUE_TYPE_RANGE);
    assert(set->param1_values[0].range.min == 1 && set->param1_values[0].range.max == 8);
    struct behavior_parameter_metadata empty = {.sets_len = 1};
    assert(space->get_parameter_metadata(NULL, &empty) == 0 && empty.sets_len == 0);
    metadata = mode->parameter_metadata;
    assert(metadata && metadata->sets_len == 1);
    set = metadata->sets;
    assert(set->param1_values_len == 1 && set->param2_values_len == 0);
    assert(set->param1_values[0].type == BEHAVIOR_PARAMETER_VALUE_TYPE_LAYER_ID);
    empty.sets_len = 1;
    assert(settings->get_parameter_metadata(NULL, &empty) == 0 && empty.sets_len == 0);
#endif
}

int main(void) {
    test_routing();
    test_invalid_parameters();
    test_non_chord_behaviors();
    test_metadata();
    return 0;
}

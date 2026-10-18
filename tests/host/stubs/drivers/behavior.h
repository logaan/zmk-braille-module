// SPDX-License-Identifier: MIT
#pragma once

/* Tiny compile-test shim, NOT a Zephyr device or runtime implementation. */
#include <stddef.h>
#include <zmk/behavior.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

enum behavior_locality {
    BEHAVIOR_LOCALITY_CENTRAL,
    BEHAVIOR_LOCALITY_EVENT_SOURCE,
    BEHAVIOR_LOCALITY_GLOBAL
};

#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
struct device;
struct behavior_parameter_value_metadata {
    char *display_name;
    union {
        uint32_t value;
        struct {
            int32_t min;
            int32_t max;
        } range;
    };
    enum { BEHAVIOR_PARAMETER_VALUE_TYPE_RANGE } type;
};
struct behavior_parameter_metadata_set {
    size_t param1_values_len;
    const struct behavior_parameter_value_metadata *param1_values;
    size_t param2_values_len;
    const struct behavior_parameter_value_metadata *param2_values;
};
struct behavior_parameter_metadata {
    size_t sets_len;
    const struct behavior_parameter_metadata_set *sets;
};
int zmk_behavior_get_empty_param_metadata(const struct device *dev,
                                          struct behavior_parameter_metadata *metadata);
#endif

struct behavior_driver_api {
    enum behavior_locality locality;
    int (*binding_pressed)(struct zmk_behavior_binding *, struct zmk_behavior_binding_event);
    int (*binding_released)(struct zmk_behavior_binding *, struct zmk_behavior_binding_event);
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    int (*get_parameter_metadata)(const struct device *, struct behavior_parameter_metadata *);
    const struct behavior_parameter_metadata *parameter_metadata;
#endif
};

#define BEHAVIOR_DT_DEFINE(node, init, pm, data, config, level, priority, api)                     \
    static const struct behavior_driver_api *const node = api

#ifndef TEST_NO_NODES
#define TEST_NO_NODES 0
#endif
#ifndef TEST_DOT_NODES
#define TEST_DOT_NODES (!TEST_NO_NODES)
#endif
#ifndef TEST_SPACE_NODES
#define TEST_SPACE_NODES (!TEST_NO_NODES)
#endif
#ifndef TEST_MODE_NODES
#define TEST_MODE_NODES (!TEST_NO_NODES)
#endif
#ifndef TEST_SETTINGS_NODES
#define TEST_SETTINGS_NODES (!TEST_NO_NODES)
#endif
#define TEST_HAS_zmk_behavior_braille_dot TEST_DOT_NODES
#define TEST_HAS_zmk_behavior_braille_space TEST_SPACE_NODES
#define TEST_HAS_zmk_behavior_braille_mode TEST_MODE_NODES
#define TEST_HAS_zmk_behavior_braille_settings TEST_SETTINGS_NODES
#define DT_HAS_COMPAT_STATUS_OKAY(compat) TEST_HAS_##compat
#define DT_FOREACH_STATUS_OKAY(compat, fn) TEST_NODES_##compat(fn)
#if TEST_DOT_NODES
#define TEST_NODES_zmk_behavior_braille_dot(fn) fn(dot_a) fn(dot_b)
#else
#define TEST_NODES_zmk_behavior_braille_dot(fn)
#endif
#if TEST_SPACE_NODES
#define TEST_NODES_zmk_behavior_braille_space(fn) fn(space)
#else
#define TEST_NODES_zmk_behavior_braille_space(fn)
#endif
#if TEST_MODE_NODES
#define TEST_NODES_zmk_behavior_braille_mode(fn) fn(mode)
#else
#define TEST_NODES_zmk_behavior_braille_mode(fn)
#endif
#if TEST_SETTINGS_NODES
#define TEST_NODES_zmk_behavior_braille_settings(fn) fn(settings)
#else
#define TEST_NODES_zmk_behavior_braille_settings(fn)
#endif

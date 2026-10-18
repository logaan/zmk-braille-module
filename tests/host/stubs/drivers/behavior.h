// SPDX-License-Identifier: MIT
#pragma once

/* Tiny compile-test shim, NOT a Zephyr device or runtime implementation. */
#include <stddef.h>
#include <stdint.h>

struct zmk_behavior_binding {
    const char *behavior_dev;
    uint32_t param1;
    uint32_t param2;
};

struct zmk_behavior_binding_event {
    int layer;
    uint32_t position;
    int64_t timestamp;
};

enum behavior_locality {
    BEHAVIOR_LOCALITY_CENTRAL,
    BEHAVIOR_LOCALITY_EVENT_SOURCE,
    BEHAVIOR_LOCALITY_GLOBAL
};

struct behavior_driver_api {
    enum behavior_locality locality;
    int (*binding_pressed)(struct zmk_behavior_binding *, struct zmk_behavior_binding_event);
    int (*binding_released)(struct zmk_behavior_binding *, struct zmk_behavior_binding_event);
};

#define BEHAVIOR_DT_DEFINE(node, init, pm, data, config, level, priority, api)                       \
    static const struct behavior_driver_api *const node = api

#define DT_HAS_COMPAT_STATUS_OKAY(compat) (!TEST_NO_NODES)
#define DT_FOREACH_STATUS_OKAY(compat, fn) TEST_NODES_##compat(fn)
#if TEST_NO_NODES
#define TEST_NODES_zmk_behavior_braille_dot(fn)
#define TEST_NODES_zmk_behavior_braille_space(fn)
#define TEST_NODES_zmk_behavior_braille_mode(fn)
#define TEST_NODES_zmk_behavior_braille_settings(fn)
#else
#define TEST_NODES_zmk_behavior_braille_dot(fn) fn(dot_a) fn(dot_b)
#define TEST_NODES_zmk_behavior_braille_space(fn) fn(space)
#define TEST_NODES_zmk_behavior_braille_mode(fn) fn(mode)
#define TEST_NODES_zmk_behavior_braille_settings(fn) fn(settings)
#endif

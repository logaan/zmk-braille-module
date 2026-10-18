// SPDX-License-Identifier: MIT
#pragma once

/* Compile-test types only. Native firmware tests use the real ZMK headers. */
#include <stdbool.h>
#include <stdint.h>

#ifndef CONFIG_ZMK_SPLIT
#define CONFIG_ZMK_SPLIT 0
#endif
#ifndef CONFIG_ZMK_BEHAVIOR_METADATA
#define CONFIG_ZMK_BEHAVIOR_METADATA 0
#endif
#define IS_ENABLED(config) (config)
#define ZMK_BEHAVIOR_OPAQUE 0
#define ZMK_BEHAVIOR_TRANSPARENT 1
#define DT_NODELABEL(name) name
#define TEST_STRINGIFY(node) #node
#define DEVICE_DT_NAME(node) TEST_STRINGIFY(node)

struct zmk_behavior_binding {
    const char *behavior_dev;
    uint32_t param1;
    uint32_t param2;
};

struct zmk_behavior_binding_event;
int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event, bool pressed);

struct zmk_behavior_binding_event {
    int layer;
    uint32_t position;
    int64_t timestamp;
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
    uint8_t source;
#endif
};

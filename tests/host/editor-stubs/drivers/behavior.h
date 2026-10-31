// SPDX-License-Identifier: MIT
#pragma once
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <zmk/behavior.h>
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
struct behavior_parameter_value_metadata {
  char *display_name;
  union {
    uint32_t value;
    struct {
      int32_t min, max;
    } range;
  };
  enum {
    BEHAVIOR_PARAMETER_VALUE_TYPE_NIL,
    BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
    BEHAVIOR_PARAMETER_VALUE_TYPE_RANGE,
    BEHAVIOR_PARAMETER_VALUE_TYPE_HID_USAGE,
    BEHAVIOR_PARAMETER_VALUE_TYPE_LAYER_ID
  } type;
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
struct device {
  const char *name;
  bool ready;
  const struct behavior_parameter_metadata *metadata;
};
struct zmk_behavior_ref {
  const struct device *device;
  struct {
    const char *display_name;
  } metadata;
};
extern struct zmk_behavior_ref registry[];
extern size_t registry_size;
#define STRUCT_SECTION_FOREACH(type, item)                                     \
  for (struct type *item = registry; item < registry + registry_size; ++item)
static inline bool device_is_ready(const struct device *d) { return d->ready; }
static inline int
behavior_get_parameter_metadata(const struct device *d,
                                struct behavior_parameter_metadata *m) {
  if (!d->metadata)
    return -ENODEV;
  *m = *d->metadata;
  return 0;
}
const struct device *zmk_behavior_get_binding(const char *id);

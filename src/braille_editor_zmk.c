#ifdef CONFIG_ZMK_BRAILLE_SETTINGS_UI
// SPDX-License-Identifier: MIT
#include <braille_editor.h>
#include <drivers/behavior.h>
#include <dt-bindings/zmk/keys.h>
#include <string.h>
#if IS_ENABLED(CONFIG_ZMK_BLE)
#include <zmk/ble.h>
#endif
static const struct {
  const char *name;
  uint32_t value;
} keycodes[] = {
#include <braille_keycodes.inc>
};
size_t braille_editor_key_count(void) { return ARRAY_SIZE(keycodes); }
bool braille_editor_key(size_t i, struct braille_editor_candidate *out) {
  if (i >= ARRAY_SIZE(keycodes))
    return false;
  *out = (struct braille_editor_candidate){.name = keycodes[i].name,
                                           .id = keycodes[i].name,
                                           .value = keycodes[i].value};
  return true;
}
static const struct zmk_behavior_ref *ref(size_t index) {
  STRUCT_SECTION_FOREACH(zmk_behavior_ref, item) {
    if (!index--)
      return item;
  }
  return NULL;
}
size_t braille_editor_behavior_count(void) {
  size_t n = 0;
  STRUCT_SECTION_FOREACH(zmk_behavior_ref, item) {
    (void)item;
    ++n;
  }
  return n;
}
static bool nil(const struct behavior_parameter_value_metadata *v, size_t n) {
  return !n || (n == 1 && v[0].type == BEHAVIOR_PARAMETER_VALUE_TYPE_NIL);
}
static bool hid(const struct behavior_parameter_value_metadata *v, size_t n) {
  return n == 1 && v[0].type == BEHAVIOR_PARAMETER_VALUE_TYPE_HID_USAGE;
}
static bool command_set(const struct behavior_parameter_metadata_set *s) {
  if (!s->param1_values_len)
    return false;
  for (size_t i = 0; i < s->param1_values_len; ++i)
    if (s->param1_values[i].type != BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE ||
        !s->param1_values[i].display_name)
      return false;
  return nil(s->param2_values, s->param2_values_len) ||
         (s->param2_values_len == 1 &&
          s->param2_values[0].type == BEHAVIOR_PARAMETER_VALUE_TYPE_RANGE &&
          s->param2_values[0].range.min >= 0 &&
          s->param2_values[0].range.max <= 255);
}
/* Exclude custom-named instances too, not just standard labels. */
static bool excluded(const char *id) {
#ifdef DT_FOREACH_STATUS_OKAY
#define EXCLUDE_NODE(node)                                                     \
  if (!strcmp(id, DEVICE_DT_NAME(node)))                                       \
    return true;
  DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_dot, EXCLUDE_NODE)
  DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_space, EXCLUDE_NODE)
  DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_mode, EXCLUDE_NODE)
  DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_settings, EXCLUDE_NODE)
  DT_FOREACH_STATUS_OKAY(zmk_behavior_transparent, EXCLUDE_NODE)
#undef EXCLUDE_NODE
#endif
  return !strncmp(id, "braille_", 8) || !strcmp(id, "transparent");
}
bool braille_editor_behavior(size_t index,
                             struct braille_editor_candidate *out) {
  const struct zmk_behavior_ref *r = ref(index);
  if (!r || !device_is_ready(r->device))
    return false;
  const char *id = r->device->name;
  if (excluded(id))
    return false;
  struct behavior_parameter_metadata m;
  if (behavior_get_parameter_metadata(r->device, &m) < 0)
    return false;
  unsigned parameters = 0;
  bool commands = false;
  if (m.sets_len) {
    if (m.sets_len == 1 &&
        (nil(m.sets[0].param1_values, m.sets[0].param1_values_len) ||
         hid(m.sets[0].param1_values, m.sets[0].param1_values_len)) &&
        (nil(m.sets[0].param2_values, m.sets[0].param2_values_len) ||
         hid(m.sets[0].param2_values, m.sets[0].param2_values_len))) {
      parameters =
          hid(m.sets[0].param1_values, m.sets[0].param1_values_len) ? 1 : 0;
      if (hid(m.sets[0].param2_values, m.sets[0].param2_values_len)) {
        if (!parameters)
          return false;
        parameters = 2;
      }
    } else {
      for (size_t i = 0; i < m.sets_len; ++i)
        if (!command_set(&m.sets[i]))
          return false;
      commands = true;
    }
  }
  *out = (struct braille_editor_candidate){
      .name = r->metadata.display_name ? r->metadata.display_name : id,
      .id = id,
      .parameters = parameters,
      .commands = commands};
  return true;
}
size_t braille_editor_command_count(const char *id) {
  struct behavior_parameter_metadata m;
  const struct device *d = zmk_behavior_get_binding(id);
  if (!d || behavior_get_parameter_metadata(d, &m) < 0)
    return 0;
  size_t n = 0;
  for (size_t i = 0; i < m.sets_len; ++i)
    n += m.sets[i].param1_values_len;
  return n;
}
bool braille_editor_command(const char *id, size_t index,
                            struct braille_editor_command *out) {
  struct behavior_parameter_metadata m;
  const struct device *d = zmk_behavior_get_binding(id);
  if (!d || behavior_get_parameter_metadata(d, &m) < 0)
    return false;
  for (size_t i = 0; i < m.sets_len; ++i) {
    const struct behavior_parameter_metadata_set *s = &m.sets[i];
    if (index >= s->param1_values_len) {
      index -= s->param1_values_len;
      continue;
    }
    *out = (struct braille_editor_command){
        .name = s->param1_values[index].display_name,
        .value = s->param1_values[index].value};
    if (!nil(s->param2_values, s->param2_values_len)) {
      out->profile = true;
      out->min = s->param2_values[0].range.min;
      out->max = s->param2_values[0].range.max;
#if IS_ENABLED(CONFIG_ZMK_BLE)
      /* ZMK BT metadata historically includes the exclusive count. */
      if (!strcmp(id, "bluetooth") && out->max >= ZMK_BLE_PROFILE_COUNT)
        out->max = ZMK_BLE_PROFILE_COUNT - 1;
#endif
    }
    return true;
  }
  return false;
}

#endif

// SPDX-License-Identifier: MIT
#include <assert.h>
#include <braille_editor.h>
#include <drivers/behavior.h>
#include <string.h>
static const struct behavior_parameter_value_metadata hid = {
    .type = BEHAVIOR_PARAMETER_VALUE_TYPE_HID_USAGE};
static const struct behavior_parameter_value_metadata layer = {
    .type = BEHAVIOR_PARAMETER_VALUE_TYPE_LAYER_ID};
static const struct behavior_parameter_value_metadata choice = {
    .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
    .value = 3,
    .display_name = "Select Profile"};
static const struct behavior_parameter_value_metadata range = {
    .type = BEHAVIOR_PARAMETER_VALUE_TYPE_RANGE, .range = {0, 4}};
static const struct behavior_parameter_metadata_set key_set = {
    .param1_values_len = 1, .param1_values = &hid};
static const struct behavior_parameter_metadata_set two_set = {
    .param1_values_len = 1,
    .param1_values = &hid,
    .param2_values_len = 1,
    .param2_values = &hid};
static const struct behavior_parameter_metadata_set layer_set = {
    .param1_values_len = 1, .param1_values = &layer};
static const struct behavior_parameter_metadata_set bt_set = {
    .param1_values_len = 1,
    .param1_values = &choice,
    .param2_values_len = 1,
    .param2_values = &range};
static const struct behavior_parameter_metadata empty = {0},
                                                key = {1, &key_set},
                                                two = {1, &two_set},
                                                layer_meta = {1, &layer_set},
                                                bt = {1, &bt_set};
static const struct device devices[] = {
    {"key_press", true, &key},     {"sticky_key", true, &key},
    {"two", true, &two},           {"reset", true, &empty},
    {"layer", true, &layer_meta},  {"missing", true, NULL},
    {"off", false, &key},          {"braille_dot", true, &empty},
    {"transparent", true, &empty}, {"bluetooth", true, &bt}};
struct zmk_behavior_ref registry[] = {
    {devices + 0, {"Key Press"}},   {devices + 1, {"Sticky Key"}},
    {devices + 2, {"Two"}},         {devices + 3, {"Reset"}},
    {devices + 4, {"Layer"}},       {devices + 5, {"Missing"}},
    {devices + 6, {"Off"}},         {devices + 7, {"Braille"}},
    {devices + 8, {"Transparent"}}, {devices + 9, {"Bluetooth"}}};
size_t registry_size = ARRAY_SIZE(registry);
const struct device *zmk_behavior_get_binding(const char *id) {
  for (unsigned i = 0; i < ARRAY_SIZE(devices); ++i)
    if (!strcmp(id, devices[i].name))
      return &devices[i];
  return NULL;
}
int main(void) {
  struct braille_editor_candidate c;
  assert(braille_editor_behavior_count() == 10);
  assert(braille_editor_behavior(0, &c) && c.parameters == 1 &&
         !strcmp(c.id, "key_press"));
  assert(braille_editor_behavior(1, &c) && c.parameters == 1);
  assert(braille_editor_behavior(2, &c) && c.parameters == 2);
  assert(braille_editor_behavior(3, &c) && c.parameters == 0);
  for (unsigned i = 4; i < 9; ++i)
    assert(!braille_editor_behavior(i, &c));
  assert(braille_editor_behavior(9, &c) && c.commands);
  struct braille_editor_command command;
  assert(braille_editor_command_count("bluetooth") == 1);
  assert(braille_editor_command("bluetooth", 0, &command) &&
         command.value == 3 && command.profile && command.max == 4);
  assert(!braille_editor_command("bluetooth", 1, &command));
  assert(braille_editor_key_count() == 2 && braille_editor_key(1, &c) &&
         !strcmp(c.name, "TAB"));
}

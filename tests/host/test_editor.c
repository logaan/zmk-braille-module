// SPDX-License-Identifier: MIT
#include <assert.h>
#include <braille_editor.h>
#include <braille_settings.h>
#include <braille_storage.h>
#include <braille_tables.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
static struct braille_rule saved[4];
static unsigned count, saves, host_output;
static int fail_save;
static struct braille_editor_candidate candidates[] = {
    {.name = "Key Press", .id = "key_press", .parameters = 1},
    {.name = "Sticky Key", .id = "sticky_key", .parameters = 1},
    {.name = "Bluetooth", .id = "bluetooth", .commands = true},
    {.name = "Reset", .id = "reset"},
    {.name = "Two Keys", .id = "two_keys", .parameters = 2},
};
size_t braille_editor_behavior_count(void) { return 6; }
bool braille_editor_behavior(size_t i, struct braille_editor_candidate *o) {
  if (i >= 5)
    return false;
  *o = candidates[i];
  return true;
}
static struct braille_editor_candidate keys[] = {
    {.name = "A", .id = "A", .value = 0x70004},
    {.name = "LEFT_ARROW", .id = "LEFT_ARROW", .value = 0x70050},
    {.name = "N4", .id = "N4", .value = 0x70021},
    {.name = "TAB", .id = "TAB", .value = 0x7002b}};
size_t braille_editor_key_count(void) { return 4; }
bool braille_editor_key(size_t i, struct braille_editor_candidate *o) {
  if (i >= 4)
    return false;
  *o = keys[i];
  return true;
}
size_t braille_editor_command_count(const char *id) {
  (void)id;
  return 2;
}
bool braille_editor_command(const char *id, size_t i,
                            struct braille_editor_command *o) {
  (void)id;
  if (i > 1)
    return false;
  *o = (struct braille_editor_command){.name = i ? "Select Profile"
                                                 : "Next Profile",
                                       .value = i ? 3 : 1,
                                       .profile = i,
                                       .min = 0,
                                       .max = 4};
  return true;
}
int braille_storage_save(const struct braille_rule *r) {
  ++saves;
  if (fail_save)
    return fail_save;
  assert(count < 4);
  saved[count++] = *r;
  return 0;
}
size_t braille_storage_count(void) { return count; }
int braille_storage_rule(size_t i, struct braille_rule *o) {
  if (i >= count)
    return -ENOENT;
  *o = saved[i];
  return 0;
}
int braille_storage_remove(const uint16_t *c, uint8_t n,
                           enum braille_word_position p) {
  assert(count && n == saved[0].chord_count && p == saved[0].word_position &&
         !memcmp(c, saved[0].chords, n * 2));
  --count;
  return 0;
}
int braille_storage_reset(void) {
  count = 0;
  return 0;
}
int braille_storage_last_error(void) { return 0; }
int braille_engine_commit_rule(const struct braille_rule *r,
                               enum braille_commit_cause cause,
                               struct zmk_behavior_binding_event e) {
  (void)r;
  (void)cause;
  (void)e;
  ++host_output;
  return 0;
}
static struct zmk_behavior_binding_event e;
static void chord(uint16_t c) {
  assert(!braille_input_dispatch(c, e));
  assert(!host_output);
}
static const uint16_t letters[] = {1,  3,  9,  25, 17, 11, 27, 19, 10,
                                   26, 5,  7,  13, 29, 21, 15, 31, 23,
                                   14, 30, 37, 39, 58, 45, 61, 53};
static void text(const char *s) {
  for (; *s; ++s)
    chord(*s == ' ' ? 0x100 : letters[*s - 'a']);
}
static void is(enum braille_editor_screen s) {
  struct braille_editor_snapshot snap;
  braille_editor_snapshot(&snap);
  assert(snap.active && snap.screen == s);
}
static void enter(void) {
  assert(!braille_settings_enter(e));
  is(BR_ED_HOME);
}
static void capture(uint16_t c) {
  text("n");
  is(BR_ED_CAPTURE);
  chord(c);
  is(BR_ED_CAPTURE_MORE);
  text("f");
  struct braille_editor_snapshot snap;
  braille_editor_snapshot(&snap);
  if (snap.screen == BR_ED_COLLISION)
    text("o");
  is(BR_ED_POSITION);
  text("a");
  is(BR_ED_BEHAVIOR);
}
static void key_action(void) {
  text("key");
  chord(0x111);
  is(BR_ED_KEY);
  text("a");
  chord(0x111);
  is(BR_ED_MODIFIERS);
  text("cd");
  is(BR_ED_ACTION_MORE);
}
int main(void) {
  struct braille_search_result result[2];
  assert(braille_search_score("kp", "Key Press") > INT_MIN);
  assert(braille_search_score("key press", "Key_Press") > INT_MIN);
  assert(braille_search_score("a ab", "a aab") > INT_MIN);
  assert(braille_search_score("a ab", "A_AAB") > INT_MIN);
  assert(braille_search_score("a ab", "A-AAB") > INT_MIN);
  assert(braille_search_score("a ab", "aaab") == INT_MIN);
  assert(braille_search_score("zz", "Key Press") == INT_MIN);
  assert(braille_search("", 6, braille_editor_behavior, result, 2) == 2 &&
         result[0].index == 2);
  assert(braille_search("key", 6, braille_editor_behavior, result, 2) == 2 &&
         result[0].index == 0);
  assert(!braille_tables_select("computer8"));
  struct braille_override_layer overrides = braille_tables_overrides();
  overrides.rules[0] =
      (struct braille_rule){.chords = {5},
                            .chord_count = 1,
                            .actions = {{.behavior_dev = "reset"}},
                            .action_count = 1,
                            .class = BRAILLE_CLASS_NEUTRAL};
  *overrides.count =
      1; /* Search k must remain text despite a dangerous override. */
  enter();
  text("m");
  is(BR_ED_MODES);
  text("c");
  assert(!strcmp(braille_tables_active(), "computer8"));
  text("u");
  assert(!strcmp(braille_tables_active(), "ueb1"));
  chord(0x103);
  is(BR_ED_HOME);
  capture(0x103);
  key_action();
  text("f");
  is(BR_ED_CONFIRM);
  assert(!saves);
  text("c");
  is(BR_ED_HOME);
  assert(!saves);
  capture(0x103);
  key_action();
  text("f");
  fail_save = -ENOSPC;
  text("s");
  is(BR_ED_CONFIRM);
  assert(saves == 1 && !count);
  struct braille_editor_snapshot snap;
  braille_editor_snapshot(&snap);
  assert(snap.error == -ENOSPC && strstr(snap.body, "key_press"));
  fail_save = 0;
  text("s");
  assert(!braille_settings_active() && count == 1);
  assert(saved[0].chords[0] == 0x103 &&
         saved[0].actions[0].param1 == 0x01070004 &&
         saved[0].class == BRAILLE_CLASS_NEUTRAL);
  enter();
  text("u");
  chord(0x111);
  is(BR_ED_REMOVE_CONFIRM);
  text("r");
  assert(!count);
  is(BR_ED_HOME);
  text("n");
  for (unsigned i = 0; i < 8; ++i) {
    chord(0x13f);
    if (i < 7) {
      is(BR_ED_CAPTURE_MORE);
      text("a");
    }
  }
  is(BR_ED_POSITION);
  text("a");
  text("blu");
  chord(0x111);
  is(BR_ED_COMMAND);
  chord(0x108);
  chord(0x111);
  is(BR_ED_PROFILE);
  chord(0x108);
  chord(0x111);
  text("f");
  text("s");
  assert(count == 1 && saved[0].chord_count == 8 &&
         saved[0].actions[0].param1 == 3 && saved[0].actions[0].param2 == 1);
  enter();
  text("r");
  is(BR_ED_RESET_CONFIRM);
  text("c");
  assert(count == 1);
  text("r");
  text("r");
  assert(!count);
  capture(0x1ff);
  text("two");
  chord(0x111);
  is(BR_ED_KEY);
  text("a");
  chord(0x111);
  text("d");
  is(BR_ED_KEY);
  text("tab");
  chord(0x111);
  text("sd");
  text("f");
  text("s");
  assert(saved[0].actions[0].param1 == 0x70004 &&
         saved[0].actions[0].param2 == 0x0207002b);
  enter();
  capture(0x1fe);
  text("key");
  chord(0x119);
  braille_editor_snapshot(&snap);
  assert(!strcmp(snap.query, "ke"));
  chord(0x103);
  text("d");
  assert(!braille_settings_active());
  enter();
  text("rr");
  capture(0x1fd);
  text("reset");
  chord(0x111);
  for (unsigned i = 1; i < 16; ++i) {
    text("a");
    text("reset");
    chord(0x111);
  }
  text("f");
  is(BR_ED_CONFIRM);
  for (unsigned i = 0; i < 15; ++i)
    chord(0x108);
  braille_editor_snapshot(&snap);
  assert(strstr(snap.body, "> 16 reset"));
  text("s");
  assert(saved[0].action_count == 16);
  enter();
  capture(0x1fc);
  text("key");
  chord(0x111);
  text("n");
  chord(0x3c);
  chord(0x19);
  braille_editor_snapshot(&snap);
  assert(!strcmp(snap.query, "n4"));
  chord(0x13f);
  assert(!braille_settings_active());
  puts("settings capture/search/parameters/confirmation/no-output passed");
}

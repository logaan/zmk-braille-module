// SPDX-License-Identifier: MIT
#include <assert.h>
#include <braille_storage.h>
#include <braille_tables.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
static uint8_t disk[256][BRAILLE_STORAGE_RECORD_MAX];
static size_t sizes[256];
static int fail, unavailable, notices;
static char saved_mode[32];
static const char *active;
static struct braille_mode modes[] = {{.name = "ueb1"}, {.name = "computer8"}};
const struct braille_mode *braille_tables_find(const char *name) {
  for (unsigned i = 0; i < 2; i++)
    if (name && !strcmp(name, modes[i].name))
      return &modes[i];
  return NULL;
}
int braille_tables_select(const char *name) {
  if (!braille_tables_find(name))
    return -ENOENT;
  active = braille_tables_find(name)->name;
  braille_storage_mode_changed(active);
  return 0;
}
int braille_tables_select_default(void) {
  return braille_tables_select("ueb1");
}
void braille_engine_reset(void) {}
int braille_storage_resolve(struct zmk_behavior_binding *b) {
  if (unavailable || strcmp(b->behavior_dev, "key_press"))
    return -ENODEV;
  if (b->param1 == 0xffffffff)
    return -EINVAL;
  b->behavior_dev = "key_press";
  return 0;
}
int braille_storage_write(unsigned slot, const uint8_t *data, size_t len) {
  if (fail)
    return -EIO;
  memcpy(disk[slot], data, len);
  sizes[slot] = len;
  return 0;
}
int braille_storage_delete(unsigned slot) {
  if (fail)
    return -EIO;
  sizes[slot] = 0;
  return 0;
}
int braille_storage_write_mode(const char *name) {
  if (fail)
    return -EIO;
  strcpy(saved_mode, name);
  return 0;
}
void braille_storage_notice(unsigned n, int e) {
  (void)e;
  notices = n;
}
static void reboot(void) {
  braille_storage_begin_load();
  for (unsigned i = 0; i < 256; i++)
    if (sizes[i])
      braille_storage_load_record(i, disk[i], sizes[i]);
  braille_storage_load_mode(saved_mode);
  braille_storage_finish_load();
}
static struct braille_rule rule(unsigned cell) {
  return (struct braille_rule){
      .chords = {cell},
      .chord_count = 1,
      .action_count = 1,
      .class = BRAILLE_CLASS_NEUTRAL,
      .actions = {{.behavior_dev = "key_press", .param1 = 17, .param2 = 29}}};
}
int main(void) {
  reboot();
  assert(!strcmp(active, "ueb1"));
  assert(braille_storage_count() == 0);
  struct braille_rule r = rule(1), out;
  r.chord_count = 8;
  r.action_count = 16;
  r.word_position = BRAILLE_WORD_POSITION_END;
  for (unsigned i = 0; i < 8; i++)
    r.chords[i] = i + 1;
  for (unsigned i = 0; i < 16; i++)
    r.actions[i] = (struct zmk_behavior_binding){
        .behavior_dev = "key_press", .param1 = i, .param2 = 100 + i};
  assert(!braille_storage_save(&r));
  assert(sizes[0] == 22 + 16 * 18);
  reboot();
  assert(!braille_storage_rule(0, &out));
  assert(out.chord_count == 8 && out.action_count == 16 &&
         out.word_position == BRAILLE_WORD_POSITION_END);
  for (unsigned i = 0; i < 16; i++)
    assert(out.actions[i].param1 == i && out.actions[i].param2 == 100 + i &&
           !strcmp(out.actions[i].behavior_dev, "key_press"));
  assert(!braille_storage_reset());
  assert(!sizes[0]);
  for (unsigned i = 1; i <= 256; i++) {
    r = rule(i);
    assert(!braille_storage_save(&r));
  }
  assert(braille_storage_count() == 256);
  r = rule(257);
  assert(braille_storage_save(&r) == -ENOSPC);
  r = rule(1);
  r.actions[0].param1 = 123;
  assert(!braille_storage_save(&r));
  reboot();
  assert(braille_storage_count() == 256);
  assert(!braille_storage_rule(0, &out) && out.actions[0].param1 == 123);
  fail = 1;
  r.actions[0].param1 = 321;
  assert(braille_storage_save(&r) == -EIO);
  assert(!braille_storage_rule(0, &out) && out.actions[0].param1 == 123);
  assert(braille_storage_remove(r.chords, 1, 0) == -EIO);
  fail = 0;
  assert(!braille_storage_remove(r.chords, 1, 0));
  assert(braille_storage_count() == 255);
  assert(!braille_storage_reset());
  r = rule(1);
  assert(!braille_storage_save(&r));
  r = rule(2);
  assert(!braille_storage_save(&r));
  /* Future-version record retains identity and storage, unrelated valid loads.
   */
  disk[0][2] = 99;
  reboot();
  assert(braille_storage_count() == 1 && braille_storage_skipped_count() == 1 &&
         notices == 1 && sizes[0]);
  r = rule(1);
  assert(!braille_storage_save(&r));
  assert(braille_storage_count() == 2 && disk[0][2] == 1 &&
         braille_storage_skipped_count() == 0);
  unavailable = 1;
  reboot();
  assert(braille_storage_count() == 0 && braille_storage_skipped_count() == 2 &&
         sizes[0] && sizes[1]);
  unavailable = 0;
  reboot();
  assert(braille_storage_count() == 2);
  disk[0][2] = 88;
  reboot();
  r = rule(1);
  assert(!braille_storage_remove(r.chords, 1, 0));
  assert(!sizes[0] && braille_storage_skipped_count() == 0);
  /* Corrupt, invalid parameters and malformed action names are isolated. */
  disk[1][0] = 0;
  reboot();
  assert(braille_storage_count() == 0 && braille_storage_skipped_count() == 1);
  assert(!braille_storage_remove_slot(1));
  r = rule(3);
  assert(!braille_storage_save(&r));
  disk[0][18] = 255;
  disk[0][19] = 255;
  disk[0][20] = 255;
  disk[0][21] = 255;
  reboot();
  assert(braille_storage_count() == 0 && braille_storage_skipped_count() == 1);
  assert(!braille_storage_reset());
  assert(!braille_tables_select("computer8"));
  assert(!strcmp(saved_mode, "computer8"));
  reboot();
  assert(!strcmp(active, "computer8"));
  strcpy(saved_mode, "ueb2");
  reboot();
  assert(!strcmp(active, "ueb1"));
  fail = 1;
  assert(!braille_tables_select("computer8"));
  assert(!strcmp(active, "computer8") && braille_storage_last_error() == -EIO);
  fail = 0;
  puts("storage: capacity, roundtrip, durable errors, skipped recovery, "
       "removal/reset and modes PASS");
  return 0;
}

// SPDX-License-Identifier: MIT
#include <assert.h>
#include <braille_chord.h>
#include <braille_storage.h>
#include <braille_tables.h>
#include <errno.h>
#include <string.h>
static unsigned output;
int braille_engine_commit_rule(const struct braille_rule *r,
                               enum braille_commit_cause cause,
                               struct zmk_behavior_binding_event event) {
  (void)cause;
  (void)event;
  if (r->action_count)
    output = r->actions[0].param1;
  return 0;
}
int braille_storage_resolve(struct zmk_behavior_binding *b) {
  return !b->behavior_dev ? -EINVAL : 0;
}
int braille_storage_write(unsigned slot, const uint8_t *data, size_t n) {
  (void)slot;
  (void)data;
  (void)n;
  return 0;
}
int braille_storage_delete(unsigned slot) {
  (void)slot;
  return 0;
}
int braille_storage_write_mode(const char *name) {
  (void)name;
  return 0;
}
static void chord(uint16_t mask) {
  assert(!braille_chord_resolved(mask, (struct zmk_behavior_binding_event){0}));
}
int main(void) {
  braille_storage_begin_load();
  braille_storage_finish_load();
  struct braille_rule r = {
      .chords = {1},
      .chord_count = 1,
      .action_count = 1,
      .class = BRAILLE_CLASS_NEUTRAL,
      .actions = {{.behavior_dev = "key_press", .param1 = 999}}};
  assert(!braille_storage_save(&r));
  for (unsigned i = 0; i < braille_tables_count(); i++) {
    assert(!braille_tables_select(braille_tables_mode(i)->name));
    output = 0;
    chord(1);
    chord(0x100);
    assert(output != 0);
    /* A direct one-cell override must replace the factory letter. */
    braille_engine_reset();
    output = 0;
    chord(1);
    if (strcmp(braille_tables_active(), "ueb2"))
      assert(output == 999);
  }
  assert(!braille_tables_select("computer8"));
  r.chords[1] = 3;
  r.chord_count = 2;
  r.actions[0].param1 = 1000;
  assert(!braille_storage_save(&r));
  output = 0;
  chord(1);
  assert(output == 0);
  chord(3);
  assert(output == 1000);
  assert(!braille_storage_remove(r.chords, 2, 0));
  output = 0;
  chord(1);
  assert(output == 999);
  assert(!braille_storage_reset());
  output = 0;
  chord(1);
  assert(output != 999 && output != 0);
  /* Fill bounded action pool, then reject another large rule without losing
   * any. */
  r.chord_count = 1;
  r.action_count = 16;
  for (unsigned j = 0; j < 16; j++)
    r.actions[j] =
        (struct zmk_behavior_binding){.behavior_dev = "key_press", .param1 = j};
  unsigned count = 0;
  for (unsigned i = 1; i < 256; i++) {
    r.chords[0] = i;
    int rc = braille_storage_save(&r);
    if (rc == -ENOSPC)
      break;
    assert(!rc);
    count++;
  }
  assert(count > 0 && count < 256 && braille_storage_count() == count);
  struct braille_rule out;
  assert(!braille_storage_rule(0, &out) && out.action_count == 16);
  return 0;
}

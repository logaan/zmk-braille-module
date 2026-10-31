// SPDX-License-Identifier: MIT
#include <braille_storage.h>
#include <braille_tables.h>
#include <errno.h>
#include <string.h>

/* Header identity remains readable for unsupported versions so a user may
 * replace/remove a temporarily unloadable binding by its input sequence. */
struct slot_info {
  uint16_t chords[8];
  uint8_t count, position, actions;
  uint16_t offset, bytes;
  bool occupied, valid;
};
static struct slot_info slots[BRAILLE_STORAGE_SLOTS];
#ifndef CONFIG_ZMK_BRAILLE_STORAGE_POOL_SIZE
#define CONFIG_ZMK_BRAILLE_STORAGE_POOL_SIZE 16384
#endif
static uint8_t pool[CONFIG_ZMK_BRAILLE_STORAGE_POOL_SIZE];
static size_t pool_used;
static uint32_t revision;
uint32_t braille_storage_revision(void) { return revision; }
size_t braille_storage_count(void) {
  size_t count = 0;
  for (unsigned i = 0; i < BRAILLE_STORAGE_SLOTS; i++)
    count += slots[i].valid;
  return count;
}
int braille_storage_rule(size_t index, struct braille_rule *out) {
  if (!out)
    return -EINVAL;
  for (unsigned i = 0; i < BRAILLE_STORAGE_SLOTS; i++)
    if (slots[i].valid) {
      if (index-- != 0)
        continue;
      const struct slot_info *s = &slots[i];
      *out = (struct braille_rule){.class = BRAILLE_CLASS_NEUTRAL,
                                   .chord_count = s->count,
                                   .action_count = s->actions,
                                   .word_position = s->position};
      memcpy(out->chords, s->chords, sizeof(s->chords));
      memcpy(out->actions, pool + s->offset, s->bytes);
      return 0;
    }
  return -ENOENT;
}
static unsigned skipped;
static int last_error;
static bool loading = true;
static uint8_t wire[BRAILLE_STORAGE_RECORD_MAX];
#define WEAK __attribute__((weak))
WEAK int braille_storage_resolve(struct zmk_behavior_binding *binding) {
  (void)binding;
  return -ENOTSUP;
}
WEAK int braille_storage_write(unsigned slot, const uint8_t *data,
                               size_t size) {
  (void)slot;
  (void)data;
  (void)size;
  return -ENOTSUP;
}
WEAK int braille_storage_delete(unsigned slot) {
  (void)slot;
  return -ENOTSUP;
}
WEAK int braille_storage_write_mode(const char *name) {
  (void)name;
  return -ENOTSUP;
}
WEAK void braille_storage_notice(unsigned count, int error) {
  (void)count;
  (void)error;
}
static int result(int error) {
  last_error = error;
  if (error)
    braille_storage_notice(skipped, error);
  return error;
}
int braille_storage_last_error(void) { return last_error; }
unsigned braille_storage_skipped_count(void) { return skipped; }
static void put32(uint8_t *p, uint32_t v) {
  for (unsigned i = 0; i < 4; i++)
    p[i] = (uint8_t)(v >> (8 * i));
}
static uint32_t get32(const uint8_t *p) {
  return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
         ((uint32_t)p[3] << 24);
}
static bool identity(const struct slot_info *s, const uint16_t *c, uint8_t n,
                     unsigned pos) {
  return s->occupied && s->count == n && s->position == pos &&
         !memcmp(s->chords, c, n * sizeof(*c));
}
static int validate(struct braille_rule *r) {
  if (!r->chord_count || r->chord_count > 8 || r->action_count > 16 ||
      r->word_position > BRAILLE_WORD_POSITION_END || r->when.numeric ||
      r->when.caps || r->when.word_start || r->class != BRAILLE_CLASS_NEUTRAL ||
      r->sets)
    return -EINVAL;
  for (unsigned i = 0; i < r->chord_count; i++)
    if (!r->chords[i] || r->chords[i] > 0x1ff)
      return -EINVAL;
  for (unsigned i = 0; i < r->action_count; i++) {
    if (!r->actions[i].behavior_dev ||
        strlen(r->actions[i].behavior_dev) > BRAILLE_STORAGE_NAME_MAX)
      return -EINVAL;
    int rc = braille_storage_resolve(&r->actions[i]);
    if (rc)
      return rc;
  }
  return 0;
}
static size_t bytes_for(const struct braille_rule *r) {
  return r->action_count * sizeof(r->actions[0]);
}
static bool fits(unsigned slot, const struct braille_rule *r) {
  return pool_used - (slots[slot].valid ? slots[slot].bytes : 0) +
             bytes_for(r) <=
         sizeof(pool);
}
static void unpublish(unsigned slot) {
  ++revision;
  struct slot_info *s = &slots[slot];
  if (!s->valid)
    return;
  memmove(pool + s->offset, pool + s->offset + s->bytes,
          pool_used - s->offset - s->bytes);
  for (unsigned i = 0; i < BRAILLE_STORAGE_SLOTS; i++)
    if (slots[i].valid && slots[i].offset > s->offset)
      slots[i].offset -= s->bytes;
  pool_used -= s->bytes;
  s->valid = false;
  s->bytes = 0;
}
static void publish(unsigned slot, const struct braille_rule *r) {
  unpublish(slot);
  struct slot_info *s = &slots[slot];
  s->offset = pool_used;
  s->bytes = bytes_for(r);
  memcpy(pool + pool_used, r->actions, s->bytes);
  pool_used += s->bytes;
  memcpy(s->chords, r->chords, sizeof(s->chords));
  s->count = r->chord_count;
  s->position = r->word_position;
  s->actions = r->action_count;
  s->occupied = s->valid = true;
}
static size_t encode(const struct braille_rule *r) {
  wire[0] = 'B';
  wire[1] = 'R';
  wire[2] = 1;
  wire[3] = r->chord_count;
  wire[4] = (uint8_t)r->word_position;
  wire[5] = r->action_count;
  size_t p = 6;
  for (unsigned i = 0; i < r->chord_count; i++) {
    wire[p++] = (uint8_t)r->chords[i];
    wire[p++] = (uint8_t)(r->chords[i] >> 8);
  }
  for (unsigned i = 0; i < r->action_count; i++) {
    const struct zmk_behavior_binding *a = &r->actions[i];
    size_t n = strlen(a->behavior_dev);
    wire[p++] = (uint8_t)n;
    memcpy(wire + p, a->behavior_dev, n);
    p += n;
    put32(wire + p, a->param1);
    p += 4;
    put32(wire + p, a->param2);
    p += 4;
  }
  return p;
}
int braille_storage_save(const struct braille_rule *rule) {
  if (!rule)
    return result(-EINVAL);
  struct braille_rule r = *rule;
  int rc = validate(&r);
  if (rc)
    return result(rc);
  int slot = -1;
  for (unsigned i = 0; i < BRAILLE_STORAGE_SLOTS; i++)
    if (identity(&slots[i], r.chords, r.chord_count, r.word_position)) {
      slot = (int)i;
      break;
    }
  if (slot < 0)
    for (unsigned i = 0; i < BRAILLE_STORAGE_SLOTS; i++)
      if (!slots[i].occupied) {
        slot = (int)i;
        break;
      }
  if (slot < 0 || !fits((unsigned)slot, &r))
    return result(-ENOSPC);
  size_t n = encode(&r);
  rc = braille_storage_write((unsigned)slot, wire, n);
  if (rc)
    return result(rc);
  if (slots[slot].occupied && !slots[slot].valid && skipped)
    skipped--;
  publish((unsigned)slot, &r);
  braille_engine_reset();
  return result(0);
}
int braille_storage_remove_slot(unsigned slot) {
  if (slot >= BRAILLE_STORAGE_SLOTS || !slots[slot].occupied)
    return result(-ENOENT);
  int rc = braille_storage_delete(slot);
  if (rc)
    return result(rc);
  if (!slots[slot].valid && skipped)
    skipped--;
  unpublish(slot);
  memset(&slots[slot], 0, sizeof(slots[slot]));
  braille_engine_reset();
  return result(0);
}
int braille_storage_remove(const uint16_t *c, uint8_t n,
                           enum braille_word_position pos) {
  if (!c || !n || n > 8)
    return result(-EINVAL);
  for (unsigned i = 0; i < BRAILLE_STORAGE_SLOTS; i++)
    if (identity(&slots[i], c, n, pos))
      return braille_storage_remove_slot(i);
  return result(-ENOENT);
}
int braille_storage_reset(void) {
  /* Each successful deletion takes effect even if a later flash write fails. */
  for (unsigned i = 0; i < BRAILLE_STORAGE_SLOTS; i++)
    if (slots[i].occupied) {
      int rc = braille_storage_remove_slot(i);
      if (rc)
        return rc;
    }
  return result(0);
}
void braille_storage_begin_load(void) {
  ++revision;
  loading = true;
  memset(slots, 0, sizeof(slots));
  pool_used = 0;
  skipped = 0;
  last_error = 0;
  braille_tables_select_default();
}
int braille_storage_load_record(unsigned slot, const uint8_t *data,
                                size_t size) {
  if (slot >= BRAILLE_STORAGE_SLOTS)
    return -EINVAL;
  if (slots[slot].occupied && !slots[slot].valid && skipped)
    skipped--;
  unpublish(slot);
  memset(&slots[slot], 0, sizeof(slots[slot]));
  slots[slot].occupied = true;
  struct braille_rule r = {.class = BRAILLE_CLASS_NEUTRAL};
  size_t p = 6;
  int rc = -EINVAL;
  if (!data || size < 6 || data[0] != 'B' || data[1] != 'R' || !data[3] ||
      data[3] > 8 || data[4] > 3 || size < 6u + 2u * data[3])
    goto invalid;
  r.chord_count = data[3];
  r.word_position = data[4];
  r.action_count = data[5];
  for (unsigned i = 0; i < r.chord_count; i++) {
    r.chords[i] = data[p] | ((uint16_t)data[p + 1] << 8);
    p += 2;
  }
  memcpy(slots[slot].chords, r.chords, sizeof(r.chords));
  slots[slot].count = r.chord_count;
  slots[slot].position = data[4];
  if (data[2] != 1 || r.action_count > 16)
    goto invalid;
  for (unsigned i = 0; i < r.action_count; i++) {
    char name[BRAILLE_STORAGE_NAME_MAX + 1];
    if (p >= size)
      goto invalid;
    unsigned n = data[p++];
    if (!n || n > BRAILLE_STORAGE_NAME_MAX || p + n + 8 > size ||
        memchr(data + p, 0, n))
      goto invalid;
    memcpy(name, data + p, n);
    name[n] = 0;
    p += n;
    r.actions[i].behavior_dev = name;
    r.actions[i].param1 = get32(data + p);
    p += 4;
    r.actions[i].param2 = get32(data + p);
    p += 4;
    rc = braille_storage_resolve(&r.actions[i]);
    if (rc)
      goto invalid;
  }
  if (p != size) {
    rc = -EINVAL;
    goto invalid;
  }
  rc = validate(&r);
  if (rc)
    goto invalid;
  if (!fits(slot, &r)) {
    rc = -ENOSPC;
    goto invalid;
  }
  publish(slot, &r);
  return 0;
invalid:
  skipped++;
  last_error = rc ? rc : -EINVAL;
  return last_error;
}
int braille_storage_load_mode(const char *name) {
  return braille_tables_find(name) ? braille_tables_select(name)
                                   : braille_tables_select_default();
}
void braille_storage_finish_load(void) {
  loading = false;
  braille_storage_notice(skipped, last_error);
}
void braille_storage_mode_changed(const char *name) {
  if (!loading)
    result(braille_storage_write_mode(name));
}

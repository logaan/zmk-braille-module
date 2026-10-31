// SPDX-License-Identifier: MIT
#pragma once
#include <braille_rules.h>
#define BRAILLE_STORAGE_SLOTS 256
#define BRAILLE_STORAGE_NAME_MAX 63
#define BRAILLE_STORAGE_RECORD_MAX                                             \
  (22 + BRAILLE_RULE_MAX_ACTIONS * (9 + BRAILLE_STORAGE_NAME_MAX))
/* All mutating calls run on the input/system work queue, never the LVGL thread.
 * A successful save is durable before the live override changes. */
int braille_storage_save(const struct braille_rule *rule);
int braille_storage_remove(const uint16_t *chords, uint8_t count,
                           enum braille_word_position position);
int braille_storage_remove_slot(unsigned slot);
int braille_storage_reset(void);
int braille_storage_last_error(void);
unsigned braille_storage_skipped_count(void);
/* Boot/import lifecycle; invalid records remain occupied on disk. */
void braille_storage_begin_load(void);
int braille_storage_load_record(unsigned slot, const uint8_t *data,
                                size_t size);
int braille_storage_load_mode(const char *name);
void braille_storage_finish_load(void);
void braille_storage_mode_changed(const char *name);
/* Backend contract. Resolve validates parameters and replaces the name with a
 * stable compiled device name pointer. No device pointers are serialized. */
int braille_storage_resolve(struct zmk_behavior_binding *binding);
int braille_storage_write(unsigned slot, const uint8_t *data, size_t size);
int braille_storage_delete(unsigned slot);
int braille_storage_write_mode(const char *name);
void braille_storage_notice(unsigned skipped, int error);

/* Compact live pool supplies ordinary common rules to the shared matcher. */
size_t braille_storage_count(void);
int braille_storage_rule(size_t index, struct braille_rule *out);

/* Changes whenever the live override pool changes; used by read-only previews. */
uint32_t braille_storage_revision(void);
